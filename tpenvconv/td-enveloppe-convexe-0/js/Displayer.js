class Displayer {
    static defaultWindowSpace = {w: 600, h:600};
    static defaultModelSpace = { minX: 0, maxX: Displayer.width, minY: 0, maxY: Displayer.height };
    currentTest;

    constructor(_htmlElement) {
        this.canvas = document.createElement('canvas');
        this.canvas.setAttribute("width", Displayer.defaultWindowSpace.w);
        this.canvas.setAttribute("height", Displayer.defaultWindowSpace.h);
        _htmlElement.appendChild(this.canvas);

        if (this.canvas.getContext) {
            this.g2d = this.canvas.getContext("2d");
        } else {
            this.canvas.write(
                "Votre navigateur ne peut visualiser cette page correctement"
            );
        }

        this.canvas.addEventListener("click", Ev.manage(this, this.click), true);
        this.canvas.addEventListener("contextMenu", Ev.manage(this, this.click), true);

        this.lineWidth = 1;
        this.pointSize = 15; //= half width
        this.padding = 4 * this.pointSize;
        this.epsilon = 3 * this.lineWidth;
        this.colors = new Colors();

        //dimensions du display
        this.setDisplaySpace(this.canvas.width, this.canvas.height);

        //dimensions des données à afficher
        this.setModelSpace(Displayer.defaultModelSpace.minX, Displayer.defaultModelSpace.minY, Displayer.defaultModelSpace.maxX, Displayer.defaultModelSpace.maxY);

        this.init();
    }

    setTest(_t) {
        this.currentTest = _t;
    }

    getPoint(e) {
        let bb = this.canvas.getBoundingClientRect();
        let that = this;
        if (this.points)
            return this.points.filter(p => that.intersect(p, { x: e.offsetX, y: e.offsetY }));
     }

    click(e) {
        let selection = this.getPoint(e);
        if (selection.length > 0) {
            this.handleSelection(selection);
        } else {
            this.currentTest.renewData();
        };
    }

    handleSelection(selection) {

    }

    setModelSpace(minX, minY, maxX, maxY) { 
        //dimensions des données à afficher
        this.minX = minX ?? 0;
        this.maxX = maxX ?? this.canvas.width;
        this.minY = minY ?? 0;
        this.maxY = maxY ?? this.canvas.height;
    }

    setDisplaySpace(w,h) {
        //dimensions du display
        this.cWidth = w;
        this.cHeight = h;
        this.cCenterX = w / 2;
        this.cCenterY = h / 2;
   }

    init() {
        this.g2d.clearRect(0, 0, this.cWidth, this.cHeight);
        this.g2d.fillStyle = this.colors.bg;
        this.g2d.fillRect(0, 0, this.cWidth, this.cHeight);
    }

    xm2p(x) {
        return this.padding +  ((this.cWidth - 2 * this.padding) * (x - this.minX)) / (this.maxX - this.minX);
    }

    ym2p(y) {
        return this.padding + ((this.cHeight - 2 * this.padding) * (y - this.maxY)) / (this.minY - this.maxY);
    }
    // mc - { mc.x, mc.y } coordonnées de la souris dans le canvas
    // p un point affiché avec ces coordonnées modèles
    intersect(p, mc) {
        let x = this.xm2p(p.x),
            y = this.ym2p(p.y),
            r = this.pointSize + this.epsilon;
        let b =
            mc.x <= x + r && mc.x >= x - r && mc.y <= y + r && mc.y >= y - r;

        return b;
    }

    //dessin point donné par coordonnées cartésiennes (transformées en coordonnées canvas)
    mDrawPoint(p) {
        this.g2d.beginPath();
        let x = this.xm2p(p.x),
            y = this.ym2p(p.y);

        this.g2d.arc(x, y, this.pointSize, 0, Math.PI * 2, true);
        this.g2d.fillStyle = this.colors.bg;
        this.g2d.fill();
        this.g2d.strokeStyle = this.colors.fg;
        this.g2d.stroke();
        if (this.pointSize > 10 && p.label) {
            this.g2d.fillStyle = this.colors.txt;
            let dx = p.label.length * 3,
                dy = 2;
            this.g2d.fillText(p.label, x - dx, y + dy);
        } else if (!p.label) {
            console.log("drole d'oiseau que ce point la " + p);
        }
    }

    mDrawArrow(a, b) {
        let t = 0.2;
        let m1 = new Coord2D(
            a.x * t + (1 - t) * b.x,
            a.y * t + (1 - t) * b.y,
            "m"
        );

        t = 0.9;
        let m2 = new Coord2D(
            a.x * t + (1 - t) * b.x,
            a.y * t + (1 - t) * b.y,
            "m"
        );
        let c1 = m1.rotate(Math.PI / 20, b),
        c2 = m1.rotate(-Math.PI / 20, b);
        t = 0.1;
        let p = new Coord2D(
            a.x * t + (1 - t) * b.x,
            a.y * t + (1 - t) * b.y,
            "p"
        );
        this.mDrawLine(m1, m2);
        this.mDrawLine(p, c1);
        this.mDrawLine(p, c2);
        this.mDrawLine(c1, c2);
        //this.mDrawPoint(b);
        //this.mDrawPoint(a);
    }

    // dessin segment entre points en coordonnées cartésiennes
    // coordonnées du modèle traduites en coordonnées canvas
    mDrawLine(p1, p2) {
        this.g2d.strokeStyle = this.colors.lc;
        this.g2d.beginPath();
        this.g2d.moveTo(this.xm2p(p1.x), this.ym2p(p1.y));
        this.g2d.lineWidth = this.lineWidth;
        this.g2d.lineJoin = "round";
        this.g2d.lineTo(this.xm2p(p2.x), this.ym2p(p2.y));
        this.g2d.stroke();
    }

    //dessin segment entre points en coordonnées "canvas"
    pDrawLine(x1, y1, x2, y2) {
        this.g2d.strokeStyle = Couleur.lc;
        this.g2d.beginPath();
        this.g2d.moveTo(x1, y1);
        this.g2d.lineTo(x2, y2);
        this.g2d.stroke();
    }

    //dessin segment entre points en coordonnées "canvas"
    pDrawOrientedLine(x1, y1, x2, y2) {
        this.g2d.strokeStyle = Couleur.lc;
        this.g2d.beginPath();
        this.g2d.moveTo(x1, y1);

        this.g2d.lineTo(x2, y2);
        this.g2d.stroke();
    }

    setOptions(points, incrementalDrawing) {
        this.points = points ?? this.points;
        this.incrementalDrawing = incrementalDrawing ?? false;
        let pointsNumber = this.points.length;
        this.pointSize = pointsNumber < 200 ? 15 : pointsNumber < 2000 ? 2 : 1;
        if (!incrementalDrawing) this.init();
    }

    //dessin d'un ensemble de points sans dessiner de lignes entre les points
    drawPoints(points, incrementalDrawing) {
        this.setOptions(points, incrementalDrawing);
        let n = points.length;
        for (let i = 0; i < n; i++) {
            this.mDrawPoint(points[i]);
        }
    }

    //dessin d'un ensemble de points sans dessiner de lignes entre les points en marquant le point qui est le centre et le point qui est sujet à comparaison
    drawPointsComp(points, incrementalDrawing) {
        this.setOptions(points, incrementalDrawing);
        let n = points.length, p;
        for (let i = 0; i < n; i++) {
            p = points[i];
            if (p.tag == undefined) this.mDrawPoint(p);
            else {
                this.colors.save();
                this.colors.fg = (p.tag == g_state.center) ? this.colors.center : (p.tag == g_state.comp) ? this.colors.comp : this.colors.fg;
                this.colors.txt = this.colors.fg;
                this.mDrawPoint(p);
                this.colors.restore();
            }
        }
    }

    //dessin d'un ensemble de points sans dessiner de lignes entre les points
    drawTour(tour, points, incrementalDrawing) {
        this.setOptions(points, incrementalDrawing);
        this.colors.save();
        this.colors.lc = (tour == 1) ? this.colors.left : (tour == -1) ? this.colors.right : this.colors.fg;
        this.mDrawLine(points[0], points[1]);
        this.mDrawPoint(points[0]);
        this.mDrawArrow(points[1], points[2]);
        this.mDrawPoint(points[1]);

        this.colors.fg = this.colors.lc;
        this.mDrawPoint(points[2]);
        this.colors.restore();
    }

    //dessin d'un tableau de points et d'une ligne qui relie chaque
    //point à son suivant (dans le tableau) et le dernier au premier
    drawLineLoop(points, incrementalDrawing) {
        this.setOptions(points, incrementalDrawing);
        this.colors.save();
        this.colors.fg = this.colors.lc;
        let n = points.length;
        for (let i = 0; i < n; i++) {
            this.mDrawLine(points[i], points[(i + 1) % n]);
            this.mDrawPoint(points[i]);
        }
        this.mDrawPoint(points[0]);
        this.colors.restore();
    }

    //dessin d'un tableau de points et de lignes entre chaque couple
    //de points (0,1), (2,3), etc.
    drawLines(points, incrementalDrawing) {
        this.setOptions(points, incrementalDrawing);
        let n = points.length;
        for (let i = 0; i < n - 1; i += 2) {
            this.mDrawLine(points[i], points[(i + 1) % n]);
            this.mDrawPoint(points[i]);
            this.mDrawPoint(points[i + 1]);
        }
    }
}