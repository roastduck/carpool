"use strict"

const backend = require("../node_modules/carpool-backend");
const apiKey = require("./api-key.js");

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        const convertor = new BMap.Convertor();
        const GCJ_COORD = 3, BD_COORD = 5;
        const MAX_BATCH = 10;
        function getPoints(arrReq, callback, arrResp = []) {
            if (arrReq.length == 0) {
                callback(arrResp);
                return;
            }
            convertor.translate(
                arrReq.slice(0, MAX_BATCH).map((pt) => {return new BMap.Point(pt.x, pt.y);}),
                GCJ_COORD,
                BD_COORD,
                (pts) => {
                    if (pts.status == 0)
                        getPoints(arrReq.slice(MAX_BATCH), callback, arrResp.concat(pts.points));
                    else
                        console.log("Error: Cannot convert coordinate (GCJ -> Baidu)");
                }
            );
        }

        const map = new BMap.Map("map");

        function addMarks(_pts, msgs, onClick, _newLine, _oldLine, callback) {
            const _allPts = _pts.concat(_newLine, _oldLine);
            getPoints(_allPts, (allPts) => {
                const pts = allPts.slice(0, _pts.length);
                const newLine = allPts.slice(_pts.length, _pts.length + _newLine.length);
                const oldLine = allPts.slice(_pts.length + _newLine.length);

                map.clearOverlays();
                for (var i in pts) {
                    const mark = new BMap.Marker(pts[i]);
                    map.addOverlay(mark);
                    if (msgs[i])
                        mark.setLabel(new BMap.Label(msgs[i], {offset: new BMap.Size(20, -10)}));
                    if (onClick[i])
                        mark.addEventListener("click", onClick[i]);
                }
                if (newLine)
                    map.addOverlay(new BMap.Polyline(newLine, {strokeColor: "red", strokeOpacity: 0.7}));
                if (oldLine)
                    map.addOverlay(new BMap.Polyline(oldLine, {strokeColor: "blue", strokeOpacity: 0.3}));

                callback();
            });
        }

        const INIT = 0, EXT = 1, FIN = 2;
        $scope.state = INIT;
        $scope.message = [
            "Click on the map for departure point",
            "Click on the map for destination point",
            "Click on the taxi marks for details"
        ];
        $scope.resetState = () => {
            map.clearOverlays();
            $scope.state = INIT;
        };

        const minLng = backend.getMinLongitude();
        const maxLng = backend.getMaxLongitude();
        const minLat = backend.getMinLatitude();
        const maxLat = backend.getMaxLatitude();
        getPoints([{x: (minLng + maxLng) / 2, y: (minLat + maxLat) / 2}], (pts) => {
            map.centerAndZoom(pts[0], 9);
            map.enableScrollWheelZoom();
            const query = {};
            map.addEventListener("click", (e) => {
                switch ($scope.state) {
                case INIT:
                    query.st = e.point;
                    map.clearOverlays();
                    const mark = new BMap.Marker(e.point);
                    map.addOverlay(mark);
                    $scope.state = EXT;
                    $scope.$apply();
                    break;
                case EXT:
                    query.en = e.point;
                    convertor.translate([query.st, query.en], BD_COORD, GCJ_COORD, (pts) => {
                        if (pts.status != 0)
                        {
                            console.log("Error: Cannot convert coordinate (Baidu -> GCJ)");
                            return;
                        }

                        const result = backend.query(
                            pts.points[0].lng, pts.points[0].lat,
                            pts.points[1].lng, pts.points[1].lat
                        );
                        pts = [result.depart, result.dest];
                        const msgs = ["From", "To"];
                        const onClick = [null, null];
                        for (var i in result.candidates)
                            ((can) => {
                                pts.push(can.taxi);
                                msgs.push("Taxi " + i);
                                onClick.push(() => {
                                    addMarks(pts.concat(can.targets), msgs, onClick, can.newPath.pts, can.oldPath.pts, () => {});
                                });
                            })(result.candidates[i]);

                        addMarks(pts, msgs, onClick, [], [], () => {
                            $scope.state = FIN;
                            $scope.$apply();
                        });
                    });
                    break;
                case FIN:
                    break;
                default:
                    console.log("Error: Invalid state");
                }
            });
        });
    }]);

