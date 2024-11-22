class DataSet {
    static #label = "p ";
    #width = 500;
    #height = 500;
 
    constructor(dataDesc, w, h) {
         this.#width = w ?? this.#width; this.#height = h ?? this.#height;
        this.opti = { minx: 0, miny: 0, maxx: this.#width, maxy: this.#height};

        if (!dataDesc) return;
        if (dataDesc.key.includes("6")) {
            this.points = this.init6Points();
        } else if (dataDesc.title.includes("8")) {
            this.points = this.init8Points(dataDesc.label ?? DataSet.#label);
        } else if (dataDesc.title.includes("9")) {
            this.points = this.init9Points(dataDesc.label ?? DataSet.#label);
        } else if (dataDesc.title.includes("aligned") && dataDesc.title.includes("inner")) {
            this.points = this.initInnerAlignedPoints(
                dataDesc.size,
                this.#width,
                this.#height,
                dataDesc.label ?? DataSet.#label
            );
        } else if (dataDesc.title.includes("aligned")) {
            this.points = this.initAlignedPoints(
                dataDesc.size,
                this.#width,
                this.#height,
                dataDesc.label ?? DataSet.#label
            );
        } else if (dataDesc.key.includes("random")) {
            this.points = this.initRandomPointsBM(
                dataDesc.size ?? 3,
                this.#width,
                this.#height,
                dataDesc.label ?? DataSet.#label
            );
        } else if (dataDesc.key == "pp") {
            this.points = this.initTourLimite();
        }
    }

    get minX() {
        return this.opti.minx;
    }
    get maxX() {
        return this.opti.maxx;
    }
    get minY() {
        return this.opti.miny;
    }
    get maxY() {
        return this.opti.maxy;
    }

    setArea(w, h) {
        this.#width = w;
        this.#height = h;
    }

    getPoints() {
        return this.points;
    }

    getCoordsForHTML() {
        let html = "<p>";
        this.points.forEach(
            (p) =>
            (html +=
                p.label +
                " (" +
                p.getRoundedX() +
                "," +
                p.getRoundedY() +
                ")<br>")
        );
        html += "</p>";
        return html;
    }

    initTourLimite() {
        let points = [
            new Coord2D(23.6, 193.742, "a"),
            new Coord2D(224.33, 101.93, "b"),
            new Coord2D(123.965, 147.83, "c"),
            new Coord2D(269.62, 31.66, "d"),
            new Coord2D(204.44, 272.98, "e"),
            new Coord2D(137.03, 152.32, "f"),
        ];
        points[2] = Coord2D.middle(points[0], points[1], "c");
        points[5] = Coord2D.middle(points[3], points[4], "f");
        return points;
    }

    //génère un tableau de k points alignés (équi-répartis et placés à l'intérieur du segment [a,b])
    randomlyAlignedPoints(k, a, b) {
        let points = new Array();
        let t;
        for (let i = 0; i < k; i++) {
            t = i / (k + 1);
            points.push(
                new Coord2D(
                    a.x * t + b.x * (1 - t),
                    a.y * t + b.y * (1 - t),
                    a.label + "." + Math.floor(t * 100)
                )
            );
            t += t;
        }
        return points;
    }

    initRandomPoints(n, w, h, label) {
       let points = new Array();
        for (let i = 0; i < n; i++) {

            points.push(
                new Coord2D(
                    12 + (w - 40) * Math.random(),
                    20 + (h - 40) * Math.random(),
                    label + i
                )
            );
        }
        return points;
    }


    // on génère une proportion du nombre des points à générer
    // on crée une base avec l'enveloppe convexe de ces points
    // on complète avec des points placés sur les segments ayant pour extrémités le premier de l'enveloppe convexe et les autres points de l'enveloppe (en dehors de ces voisins immédiats)
    // cas particuliers:
    // pour n < 3, on ne fait rien
    // pour n == 3, on génère des triangles

    initInnerAlignedPoints(n, w, h, label) {
        let candidatePoints = this.initRandomPoints(Math.floor(n / 5), w, h, label);
        let base = (new EnveloppeConvexe(candidatePoints)).getEnvConv();
        let points = base.slice(0); //copy
        let l = base.length;
        if (l < 5) {
            base = ([base[0], base[1]].concat(candidatePoints.filter(p => (p != base[0] && p != base[1] && p != base[l-1])))).concat([base[l - 1]]);
        }
        let k = 1, j = 2;
        while (points.length < n - 1 && k <  n) {
            j = 2;
            while (j < l - 2 && points.length < n - 1) {
                points.splice(points.length - 1, 0, ...this.randomlyAlignedPoints(k, base[0], base[j++]));
            }
            k++;
        }
        let m = points.length;
        if (m >= n)
            points.splice(n);
        return points; 
    }

    // on crée une base random contenant environ 1/3 des points 
    // on complète avec des points placés sur les segments ayant pour extrémités le premier point de la base et un autre point de la base
    // cas particuliers:
    // pour n < 3, on ne fait rien
    // pour n == 3, on génère des triangles

    initAlignedPoints(n, w, h, label) {
        if (n === 3) return this.initThreeAlignedPoints(w, h, label);

        let base = this.initRandomPoints(Math.floor(n/5), w, h, label);
        let points = new Array();
        let l = base.length;
        let k = Math.floor(n/(l - 3));
        for (let i = 2; i < l-1; i++) {
                points = points.concat(this.randomlyAlignedPoints(k, base[0], base[i]));
        }
        if ((l - 3) % n != 0) { //alors on n'a généré que n - 1 points
            let c = this.getExtrema(points);
            points.push(Coord2D.middle(c.east, c.west));
        }
        points = points.concat(base);
        return points;
    }

    getExtrema(points) {
        return {
            east: this.getEast(points),
            west: this.getWest(points),
            north: this.getNorth(points),
            south: this.getSouth(points)
        }
    }

     // todo : reduce code redundancies
    getEast(points) {
        let max = points[0].x;
        let r = points[0];
        points.forEach(p => { if (p.x > max) { max = p.x; r = p; } });
        return r;
    }
    getWest(points) {
        let min = points[0].x;
        let r = points[0];
        points.forEach(p => { if (p.x < min) { min = p.x; r = p; } });
        return r;
   }
    getNorth(points) {
        let max = points[0].y;
        let r = points[0];
        points.forEach(p => { if (p.y > max) { max = p.y; r = p; } });
        return r;
    }
    getSouth(points) {
        let min = points[0].y;
        let r = points[0];
        points.forEach(p => { if (p.y < min) { min = p.x; r = p; } });
        return r;
   }

 

    //init random with boxMuller transform (source www.bit-101.com and stackOverflow)
    normalizedRandom() {
        let x = 0, y = 0;
        while (x === 0) x = Math.random();
        while (y === 0) y = Math.random();
        return Math.sqrt(-2 * Math.log(x)) * Math.cos(2 * Math.PI * y);
    }

 
    initThreeAlignedPoints( w, h, label) {
        let points = new Array();
        for (let i = 0; i < 2; i++) {
            points.push(
                new Coord2D(
                    12 + (w - 40) * Math.random(),
                    20 + (h - 40) * Math.random(),
                    label + i
                )
            );
        }
        points.push(Coord2D.middle(points[0], points[1]));
        
        return points;
    }

    // génère des valeurs dont la moyenne avoisine 0, dans un intervalle d'amplitudes random en x et en y   
    initRandomPointsBM(n, w, h, label) {
        let points = new Array();
        let p;
        let minx , miny, maxx , maxy , sumx = 0, sumy=0; 
        for (let i = 0; i < n; i++) {          
             p = new Coord2D(
                 this.normalizedRandom() ,
                 this.normalizedRandom() ,
                label + i
            );
            if (minx === undefined) {
                minx = miny = maxx = maxy = p.x;
            }
            sumx += p.x; sumy += p.y;
            if (p.x < minx) minx = p.x; if (p.y < miny) miny = p.y;
            if (p.x > maxx) maxx = p.x; if (p.y > maxy) maxy = p.y;
            points.push(p);
        }
        this.opti = { minx: minx, miny: miny, maxx: maxx, maxy: maxy };
        let moyennex = sumx / n, moyenney = sumy / n;
        return points;
    }

    init6Points(label) {
        let points = [
            new Coord2D(210, 120, label ?? "D"),
            new Coord2D(400, 130, label ?? "B"),
            new Coord2D(500, 260, label ?? "C"),
            new Coord2D(320, 540, label ?? "F"),
            new Coord2D(290, 330, label ?? "A"),
            new Coord2D(130, 260, label ?? "G"),
        ];
        return points;
    }

    init8Points(label) {
        let points = new Array();
        let x = [120, 380, 530, 340, 160, 330, 400, 255],
            y = [340, 380, 350, 180, 400, 450, 275, 459];
        for (let i = 0; i < x.length; i++) {
            points.push(new Coord2D(x[i], y[i], label + i));
        }
        return points;
    }

    init9Points(label) {
        let i = 0;
        let a = new Coord2D(20, 321.02, label + i++), b = new Coord2D(288.89, 107.48, label + i++), c = Coord2D.middle(a, b, label + i++);
        let points = [a,b,c,
            new Coord2D(236.52, 177.82, label + i++),
            new Coord2D(129.11, 476.86, label + i++),
            new Coord2D(125.12, 431.42, label + i++),

            new Coord2D(247.39, 292.86, label + i++),
            new Coord2D(291.55, 355.63, label + i++),
            new Coord2D(494.1, 387.5, label + i++),

            ]
        return points;
    }

    triRadial() {

        let origine = this.points[0];
        let points = this.points.slice(1);
        new TriRadial(points, origine);
        this.points = [origine].concat(points);
    }

    size() {
        return this.points.length;
    }
}
