 
class Option {
    key;
    title;
    description;
    selected = "";
    displayOptionInfo;
 
    constructor(_key, _title, _description) {
        this.key = _key;
        this.title = _title;
        this.description = _description;
    }

    toHtml() {
        return "<option " + this.selected + " value = \"" + this.key + "\">" + this.title + "</option>";
    }

    displayDescription() {
        return  "<p>"+this.description+"</p>";
    }

}

class DataOption extends Option {
    label;
    size;
    dataSet;
   constructor(key, title, description, label, size) {
        super(key, title, description);
        this.label = label;
        this.size = size;
        this.dataSet = new DataSet(this);
     }
    displayDescription() {
        return super.displayDescription()+"<p>" + this.dataSet.getCoordsForHTML() + "</p>";
    }
  
}

// priorite simplicite (draft version un peu contravariante...)
class Choice {
    optionArray;
    choiceElement;
    infoElement;
    currentOption;

    constructor(_options, _domElt) {
        this.choiceElement = document.createElement('select');
        this.optionArray = _options;
        this.choiceElement.innerHTML = this.toHtml();
        this.infoElement = document.createElement('p');
        _domElt.appendChild(this.choiceElement);
        _domElt.appendChild(this.infoElement);
    }

    toHtml() {
        let html = this.optionArray.reduce((past, current) => past + " " + current.toHtml(), "");
        return html;
    }

    getHtmlElement() {
        return this.choiceElement;
    }

    getValue() {
        return this.value;
    }

    update(newKey) {
        let key = newKey ? newKey : this.optionArray[0].key;
        this.optionArray.forEach(x => {
            if (x.key === key) this.currentOption = x;
        });
        this.infoElement.innerHTML = this.currentOption.displayDescription();
    }

    //ces méthodes ne sont pertinentes que pour les dataoption
    //(version draft à revoir mais pratique avec l'hypothèse -> ceux qui les utilisent le savent)
    getCurrentDataSet() {
        return this.currentOption.dataSet;
    }

    renew() {
        this.currentOption.dataSet = new DataSet(this.currentOption);
    }
}


class ControlPanel {
    dataOption;
    actionOption;
    runningOption;
    currentTest;

    constructor(_htmlElement) {
        let leftPane = document.createElement('div');
        leftPane.setAttribute("id", "leftPane");
        _htmlElement.appendChild(leftPane);
        let explicationsOrdre = "<p>Comparaison des points du plan</p><ul>\
                <li> un clic sur un point affiché le sélectionne comme centre de la comparaison,</li >\
                <li> lorsqu'un centre de comparaison est sélectionné, un nouveau clic sur un point affiché sélectionne ce point comme point de référence\
                    (c'est le point auquel les autres points sont comparés) et lance le test de comparaison. </li >\
                <li> lorsque centre de comparaison et point de référence sont sélectionnés, cliquer sur un autre point change le point de référence qui devient celui sur lequel l'utilisateur a cliqué.</li> \
                <li> un clic sur le  point sélectionné comme centre annule la sélection du centre et celle du point de référence </li></ul>";
        let algoLink = "Algorithme vu en cours (<a href=\"pdf/algos-resume-2022.pdf\" >voir le r&eacute;sum&eacute;</a>)";
        let options = [
            new Option("tour", "orientation", "<ul><li> red arrows => sens direct <br> = left turn, gauche, anti-horaire, sens trigo </li><li>green arrows => sens indirect <br> = right turn, tour droit, horaire </li> <li>white arrows => aligned </li></ul>"),
            new Option("comp", "comparaison", explicationsOrdre ),
            new Option("tri", "tri radial", explicationsOrdre),
            new Option("algo_demiPlan", "algo des demi-plans", algoLink),
            new Option("algo_jarvis", "algo de Jarvis", algoLink),
            new Option("algo_graham", "algo de Graham", algoLink)
        ];
        this.actionOption = new Choice(options, leftPane);
        this.actionOption.update();
        this.actionOption.getHtmlElement().addEventListener('change', Ev.manage(this, this.updateAction), false);


        options = [
            new DataOption("simple6", "simple data (6 points)", "simple data set, <br> (does not contain 3 or more aligned points)", "A", 6),
            new DataOption("simple8", "simple data (8 points)", "simple data set, <br> (does not contain 3 or more aligned points)", "A", 8),
            new DataOption("threeAlignedPoints", "three aligned points (9 points)", "small dataset with 3 aligned points on the convex hull", "A", 9),
            new DataOption("random3", "random (3 points)", "generates 3 points with random coordinates, left click to get a new set", "A", 3),
            new DataOption("random10", "random (10 points)", "generates 10 points with random coordinates, left click to get a new set", "A", 10),
            new DataOption("random100", "random (100 points)", "generates 100 points with random coordinates, left click to get a new set", "A", 100),
            new DataOption("random1000", "random (1 000 points)", "generates 1 000 points with random coordinates, left click to get a new set", "A", 1000),
            new DataOption("random10000", "random (10 000 points)", "generates 10 000 points with random coordinates, left click to get a new set", "A", 10000),
            new DataOption("3a", "3 aligned points", "3 aligned points, left click to get a new set", "A", 3),
            new DataOption("pp", "precision problem (tour)", "aligned points by construct (middle point), but alignement test fails", "A", 9),
            new DataOption("ia", "inner aligned points", "points with inner aligned points ", "A", 31),
            new DataOption("aa", "randomly aligned points", "points with aligned points possibly laying on the convex hull", "A", 40),
        ];

        this.dataOption = new Choice(options, leftPane);
        this.dataOption.update();
        this.dataOption.getHtmlElement().addEventListener('change', Ev.manage(this, this.updateData), false);
        options = [
            new Option("run", "unit test", "Options:<ul><li> unit test - draw result without animation</li><li> animate - animate the steps leading to the result</li>"),
            new Option("animate", "animate")
        ]
        this.runningOption = new Choice(options, leftPane);
        this.runningOption.update();
        this.runningOption.getHtmlElement().addEventListener('change', Ev.manage(this, this.updateRunningOption), false);
    }

    setTest(_t) {
        this.currentTest = _t;
    }
    
    getDataSet() {
        return this.dataOption.getCurrentDataSet();
    }

    renewData() {
        this.dataOption.renew();
    }

    updateData(e) {
        let key = e.target.value;
        this.dataOption.update(key);
        this.currentTest.refresh();
   }

    updateAction(e) {
        let key = e.target.value;
        this.actionOption.update(key);
        this.currentTest.changeAction(key);
    }

    updateRunningOption(e) {
        //todo
    }
   

 }
