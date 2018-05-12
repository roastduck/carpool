"use strict"

const backend = require("../../build/backend");

angular.module('appIndex', [])
    .controller('IndexController', ['$scope', ($scope) => {
        $scope.hello = "Hello " + backend.hello();
    }]);

