class TriRadial {
    constructor(V, o) {
        this.V = V; //array of Coord2D
        this.center = o; //Coord2D of the center chosen to sort around
        this.triParTas();
    }

    compare(a, b) {
        return -Coord2D.compare(this.center, a, b);
    }

    // tri du tableau en O(nlogn)
    triParTas() {
        this.construireTas();

        for (let k = this.V.length - 1; k >= 1; k--) {
            this.echanger(0, k);
            this.reparerTas(k, 0);
        }
    }

    indiceGauche(i) {
        return 2 * i + 1;
    }

    indiceDroit(i) {
        return 2 * i + 2;
    }

    echanger(i, j) {
        let tmp = this.V[i];
        this.V[i] = this.V[j];
        this.V[j] = tmp;
    }

    reparerTas(n, i) {
        let g = this.indiceGauche(i),
            d = this.indiceDroit(i);
        let imax = i;
        if (g < n && this.compare(this.V[g], this.V[imax]) > 0) {
            imax = g;
        }
        if (d < n && this.compare(this.V[d], this.V[imax]) > 0) {
            imax = d;
        }
        if (imax !== i) {
            this.echanger(i, imax);
            this.reparerTas(n, imax);
        }
    }

    construireTas() {
        let n = this.V.length;
        for (let k = Math.floor(n / 2) - 1; k >= 0; k--) {
            this.reparerTas(n, k);
        }
    }
}
