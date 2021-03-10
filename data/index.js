const weightElement = document.querySelector('#weight')

setInterval(function ( ) {
    fetch("/weight", {method:"GET"})
        .then((request) => {return request.json()})
        .then((json) => {weightElement.textContent=json.weight})  
}, 10000 );