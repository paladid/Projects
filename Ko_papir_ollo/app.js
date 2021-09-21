let YouScore = 0;
let PCScore = 0;
const YouScore_span = document.getElementById("You-score");
const PCScore_span = document.getElementById("PC-score");
const scoreBoard_div = document.querySelector(".score-board")
const result_div = document.querySelector(".result > p")
const rock_div = document.getElementById("k");
const paper_div = document.getElementById("p");
const scissors_div = document.getElementById("o");

function getComputerChoice() {
    const choices = ['k', 'p', 'o'];
    const randomNumber = Math.floor(Math.random() * 3);
    return choices[randomNumber];
}

function converttoword(letter) {
    if (letter === "k") return "Kő";
    if (letter === "p") return "Papír";
    return "Olló";
}

function win(YouChoice, PCChoice) {
    YouScore++;
    YouScore_span.innerHTML = YouScore;
    PCScore_span.innerHTML = PCScore;
    result_div.innerHTML = `Az általad választott ${converttoword(YouChoice)} győzelmet aratott a PC által választott ${converttoword(PCChoice)} felett`;
    document.querySelector(".score-board").classList.add('green-glow');
    setTimeout(function() { document.querySelector(".score-board").classList.remove('green-glow') }, 800);
}

function lose(YouChoice, PCChoice) {
    PCScore++;
    PCScore_span.innerHTML = PCScore;
    YouScore_span.innerHTML = YouScore;
    result_div.innerHTML = `A PC által választott ${converttoword(PCChoice)} győzelmet aratott az általad választott ${converttoword(YouChoice)} felett`;
    document.querySelector(".score-board").classList.add('red-glow');
    setTimeout(function() { document.querySelector(".score-board").classList.remove('red-glow') }, 800);
}

function draw() {
    result_div.innerHTML = `Döntetlen`;
    document.querySelector(".score-board").classList.add('grey-glow');
    setTimeout(function() { document.querySelector(".score-board").classList.remove('grey-glow') }, 800);
}

function game(YouChoice) {
    const PCChoice = getComputerChoice();
    switch (YouChoice + PCChoice) {
        case "ko":
        case "pk":
        case "op":
            win(YouChoice, PCChoice);
            break;
        case "kp":
        case "po":
        case "ok":
            lose(YouChoice, PCChoice);
            break;
        case "kk":
        case "pp":
        case "oo":
            draw(YouChoice, PCChoice);
            break;
    }
}

function main(){
    rock_div.addEventListener('click', function(){
        game("k");
    })

    paper_div.addEventListener('click', function(){
        game("p");
    })

    scissors_div.addEventListener('click', function(){
        game("o");
    })
}

main();