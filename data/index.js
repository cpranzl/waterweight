const weightElement = document.querySelector('#weight')
const ssidElement = document.querySelector('#ssid')

fetch("/ssid", {method:"GET"})
    .then((request) => {return request.json()})
    .then((json) => {ssidElement.textContent=json.ssid})

setInterval(function ( ) {
    fetch("/weight", {method:"GET"})
        .then((request) => {return request.json()})
        .then((json) => {weightElement.textContent=json.weight})
}, 10000 );
