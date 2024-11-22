class Coord2D {
    constructor(x, y, label) {
        this.x = x;
        this.y = y;
        this.label = label;
    }
    getRounded(a) {
        if (Number.isInteger(a)) return a;
        else return Math.round(a * 100) / 100;
    }
    getRoundedX() {
        return this.getRounded(this.x);
    }
    getRoundedY() {
        return this.getRounded(this.y);
    }

    //a et b sont des points du plan représentés par leurs coordonnées en 2D
    static vecteur(a, b) {
        return new Coord2D(b.x - a.x, b.y - a.y, a.label + b.label);
    }

    //a et b sont des points du plan représentés par leurs coordonnées en 2D
    static middle(a, b, label) {
        return new Coord2D((b.x + a.x) / 2, (a.y + b.y) / 2, label ?? "m");
    }

    // v1 et v2 sont des vecteurs du plan représentés par leurs coordonnées en 2D
    static determinant(v1, v2) {
        // todo
        return v1.x*v2.y-v1.y*v2.x;
    }

    // v1 et v2 sont des vecteurs du plan représentés par leurs coordonnées en 2D
    static detSign(v1, v2) {
        //todo
    }

    static tour(o, p1, p2) {
        //todo
     }

    // donne la position relative de p1 et de p2 dans un ordre trigonométrique, centré sur l'origine
    // les résultats de cette fonction sont inspirés du signe du sinus de l'angle entre les vecteurs op1 et op2:
    // 1 : lorsque p1 < p2 ( p1 *avant* p2 dans l'ordre trigonométrique sur [0, 2Pi[ )
    // -1 : lorsque p1 > p2 (p1 *après* p2 dans l'ordre trigonométrique sur [0, 2Pi[ )
    // 0 : lorsque O, p1, p2 sont alignés
    static compare(origine, p1, p2) {
        //todo
     }

    static findMinIdx(points) {
        //todo
    }

    toString() {
        return this.label + "(" + this.getRoundedX() + "," + this.getRoundedY() + ")";

    }

    //utile au dessin des fleches
    rotate(alpha, centre) {
        let result = new Coord2D(0, 0, "image+(" + this.label + ")");
        let cos = Math.cos(alpha),
            sin = Math.sin(alpha);
        result.x =
            cos * this.x -
            sin * this.y -
            cos * centre.x +
            sin * centre.y +
            centre.x;
        result.y =
            sin * this.x +
            cos * this.y -
            sin * centre.x -
            cos * centre.y +
            centre.y;
        return result;
    }
}
