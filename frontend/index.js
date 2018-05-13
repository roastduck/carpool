"use strict"

const backend = require("../node_modules/carpool-backend");
const apiKey = require("./api-key.js");

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        const convertor = new BMap.Convertor();
        const TRUE_COORD = 1;
        const BD_COORD = 5;
        function getPoints(arr, callback) {
            const ptrs = [];
            for (var i in arr) {
                ptrs.push(new BMap.Point(arr[i][0], arr[i][1]));
            }
            convertor.translate(ptrs, TRUE_COORD, BD_COORD, (ptrs) => {
                if (ptrs.status == 0)
                    callback(ptrs.points);
                else
                    console.log("Error: Cannot convert coordinate");
            });
        }

        const map = new BMap.Map("map");
        const minLng = backend.getMinLongitude();
        const maxLng = backend.getMaxLongitude();
        const minLat = backend.getMinLatitude();
        const maxLat = backend.getMaxLatitude();
        getPoints([[(minLng + maxLng) / 2, (minLat + maxLat) / 2]], (ptrs) => {
            map.centerAndZoom(ptrs[0], 9);
        });
    }]);

