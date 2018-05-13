"use strict"

const backend = require("../node_modules/carpool-backend");
const apiKey = require("./api-key.js");

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        const map = new BMap.Map("map");

        function addMarks(pts, msgs, onClick, newLine = [], oldLine = []) {
            pts = pts.map((pt) => {return new BMap.Point(pt.x, pt.y);});
            newLine = newLine.map((pt) => {return new BMap.Point(pt.x, pt.y);});
            oldLine = oldLine.map((pt) => {return new BMap.Point(pt.x, pt.y);});

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
            $scope.statReady = false;
        };
        $scope.resetState();

        const minLng = backend.getMinLongitude();
        const maxLng = backend.getMaxLongitude();
        const minLat = backend.getMinLatitude();
        const maxLat = backend.getMaxLatitude();
        map.centerAndZoom(new BMap.Point((minLng + maxLng) / 2, (minLat + maxLat) / 2), 9);
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
                const result = backend.query(
                    query.st.lng, query.st.lat,
                    query.en.lng, query.en.lat
                );
                const pts = [result.depart, result.dest];
                const msgs = ["From", "To"];
                const onClick = [null, null];
                for (var i in result.candidates)
                    ((can) => {
                        pts.push(can.taxi);
                        msgs.push("Taxi " + i);
                        onClick.push(() => {
                            addMarks(pts.concat(can.targets), msgs, onClick, can.newPath.pts, can.oldPath.pts);
                            $scope.statReady = true;
                            $scope.numOnBoard = can.numOnBoard;
                            $scope.oldDist = can.oldPath.dist;
                            $scope.pickDist = can.pickDist;
                            $scope.afterDist = can.newPath.dist - can.pickDist;
                            $scope.aloneDist = can.aloneDist;
                            $scope.ourDetour = $scope.afterDist - can.aloneDist;
                            $scope.theirDetour = can.newPath.dist - can.oldPath.dist;
                            $scope.$apply();
                        });
                    })(result.candidates[i]);

                addMarks(pts, msgs, onClick);
                $scope.state = FIN;
                $scope.$apply();
                break;
            case FIN:
                break;
            default:
                console.log("Error: Invalid state");
            }
        });
    }]);

