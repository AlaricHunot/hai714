
class EnveloppeConvexe {
    static algo = { demiPlan: 0, jarvis: 1, graham: 2 };
    constructor(lesPoints, a) {
        this.algo = a ?? EnveloppeConvexe.algo.jarvis;
        this.points = lesPoints;
        this.envConv = new Array();
        this.runAlgo(this.algo);
    }

    getEnvConv() {
         return this.envConv;
    }

    updateEnvConvFromIndices(indices) {
       this.envConv = new Array();
       for (let i = 0; i < indices.length; i++) {
           this.envConv.push(this.points[indices[i]]);
        }      
    }

    runAlgo(idAlgo) {
        this.algo = idAlgo;
        switch (this.algo) {
            case EnveloppeConvexe.algo.demiPlan:
                this.envconv = this.algoDemiPlan(this.points);
                break;
            case EnveloppeConvexe.algo.jarvis:
                this.envconv = this.algoJarvis(this.points);
                break;
            case EnveloppeConvexe.algo.graham:
                this.envconv = this.algoGraham(this.points);
                break;
            default:
                console.log("algo non défini => algo jarvis utilisé");
                this.envconv = this.algoJarvis(this.points);
                break;
        }
    }

    findNextIdx(currentIdx, points) {
     //todo: fonction utile pour l'algo de Jarvis
    }

 

    algoDemiPlan(points) {
    //todo: implementation de cet algo
    }

    algoJarvis(points) {
    //todo: implementation de cet algo
    }

    algoGraham() {
    //todo: implementation de cet algo
    }
}
