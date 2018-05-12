"use strict"

const util = require("util");
const backend = require("../node_modules/carpool-backend");
const apiKey = require("./api-key.js");

function genUrl(param) {
    return util.format(
        "%s?ak=%s&width=%d&height=%d&bbox=%d,%d;%d,%d",
        "http://api.map.baidu.com/staticimage/v2",
        apiKey,
        600, // Width
        600, // Height
        param.minLng,
        param.minLat,
        param.maxLng,
        param.maxLat
    );
}

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        var param = {
            minLng: backend.getMinLongitude(),
            maxLng: backend.getMaxLongitude(),
            minLat: backend.getMinLatitude(),
            maxLat: backend.getMaxLatitude()
        };
        $scope.mapUrl = genUrl(param);
    }]);

