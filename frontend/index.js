"use strict"

const backend = require("../node_modules/carpool-backend");
const apiKey = require("./api-key.js");

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        const convertor = new BMap.Convertor();
        const TRUE_COORD = 1, GCJ_COORD = 3, BD_COORD = 5;
        function getPoints(arr, callback) {
            const pts = [];
            for (var i in arr) {
                pts.push(new BMap.Point(arr[i].x, arr[i].y));
            }
            convertor.translate(pts, TRUE_COORD, BD_COORD, (pts) => {
                if (pts.status == 0)
                    callback(pts.points);
                else
                    console.log("Error: Cannot convert coordinate (True -> Baidu)");
            });
        }

        const map = new BMap.Map("map");

        function addMarks(_pts) {
            getPoints(_pts, (pts) => {
                for (var i in pts) {
                    const mark = new BMap.Marker(pts[i], {});
                    map.addOverlay(mark);
                }
            });
        }

        const minLng = backend.getMinLongitude();
        const maxLng = backend.getMaxLongitude();
        const minLat = backend.getMinLatitude();
        const maxLat = backend.getMaxLatitude();
        getPoints([{x: (minLng + maxLng) / 2, y: (minLat + maxLat) / 2}], (pts) => {
            map.centerAndZoom(pts[0], 9);
            map.enableScrollWheelZoom();
            map.addEventListener("click", (e) => {
                convertor.translate([e.point], BD_COORD, GCJ_COORD, (pts) => {
                    if (pts.status != 0)
                    {
                        console.log("Error: Cannot convert coordinate (Baidu -> GCJ)");
                        return;
                    }
                    const result = backend.query(pts.points[0].lng, pts.points[0].lat);
                    map.clearOverlays();
                    addMarks([result.query]);
                });
            });
        });
    }]);

