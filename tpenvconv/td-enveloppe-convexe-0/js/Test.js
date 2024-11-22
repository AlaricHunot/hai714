class Test {
    static defaultAction = "draw";
    cp; // control panel
    a;  // action
    d;  // displayer
    centre = new Coord2D(0, 0);
    referencePoint;
    states = { center: 2, comp: 3, smaller: -1, bigger: 1, equal: 0 };


    constructor(_action) {
        this.a = _action;
    }

    //priori simplicité (version draft: les clés des options ui sont utilisées telles quelles dans un if else infernal, beurk mais rapide, à revoir ?)
    //ui keys = tour, comp, tri, algo_dp, algo_jarvis, algo_graham
    run() {
        this.d.init();
        this.d.setModelSpace(...this.getModelSpace());
        this.d.drawPoints(this.getPoints(), false);

        //todo
    }

    setUI(_controlPanel, _displayer) {
        this.cp = _controlPanel;
        this.d = _displayer;
    }

    changeAction(_newA) {
        this.a = _newA;
        this.run();
    }
    
    refresh() {
        this.run();
    }

    // made to fit the signature of setModelSpace@Displayer(minX, minY, maxX, maxY)
    getModelSpace() {
        return [
            this.cp.getDataSet().minX,
            this.cp.getDataSet().minY,
            this.cp.getDataSet().maxX,
            this.cp.getDataSet().maxY
        ]
    }

    getPoints() {
        return this.cp.getDataSet().getPoints();
    }

    renewData() {
        this.cp.renewData();
        this.refresh();
    }
}