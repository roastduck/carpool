"use strict"

const backend = require("../node_modules/carpool-backend");
const apiKey = require("./api-key.js");

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        const convertor = new BMap.Convertor();
        const GCJ_COORD = 3, BD_COORD = 5;
        function getPoints(arr, callback) {
            const pts = [];
            for (var i in arr) {
                pts.push(new BMap.Point(arr[i].x, arr[i].y));
            }
            convertor.translate(pts, GCJ_COORD, BD_COORD, (pts) => {
                if (pts.status == 0)
                    callback(pts.points);
                else
                    console.log("Error: Cannot convert coordinate (True -> Baidu)");
            });
        }

        const map = new BMap.Map("map");

        function addMarks(_pts, msgs, callback) {
            getPoints(_pts, (pts) => {
                for (var i in pts) {
                    const mark = new BMap.Marker(pts[i]);
                    map.addOverlay(mark);
                    if (i < msgs.length)
                        mark.setLabel(new BMap.Label(msgs[i], {offset: new BMap.Size(20, -10)}));
                }
                callback();
            });
        }

        const INIT = 0, EXT = 1;
        $scope.state = INIT;
        $scope.message = [
            "Click on the map for departure point",
            "Click on the map for destination point"
        ];

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
                        map.clearOverlays();
                        addMarks([result.depart, result.dest], ["From", "To"], () => {
                            $scope.state = INIT;
                            $scope.$apply();
                        });
                    });
                    break;
                default:
                    console.log("Error: Invalid state");
                }
            });
        });
    }]);

