"use strict"

const backend = require("../node_modules/carpool-backend");

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        $scope.hello = "Hello " + backend.hello();
    }]);

