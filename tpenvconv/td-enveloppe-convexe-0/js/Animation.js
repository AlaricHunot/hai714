class Animation {
    constructor(maxStep) {
        this.defaultInit(maxStep);
    }

    defaultInit(
        maxStep = 20,
        id = 0,
        step = 0,
        start = 0,
        previousTimeStep = 0,
        frameDuration = 400
    ) {
        this.id = id;
        this.step = step;
        this.start = start;
        this.previousTimeStep = previousTimeStep;
        // remarques:
        // (1) duration est considéré comme variable dépendante, seules maxStep et frameDuration sont indépendantes.
        // (2) l'animation s'arrête quand maxStep est dépassé

        this.frameDuration = frameDuration;
        this.maxStep = maxStep;
        this.duration = maxStep * frameDuration;
     }

    run() {
        this.defaultInit(this.maxStep);
        let that = this;
        if (this.firstStep) this.firstStep(Date.now());
        this.id = window.requestAnimationFrame(function (t) {
            that.start = t;
            that.previousTimeStep = t;
            that.animate(t);
        });
        if (this.lastStep) this.lastStep(Date.now());
    }

    animate(t) {
        let elapsed = t - this.start;
         if (elapsed > this.duration && this.step > this.maxStep) {
            window.cancelAnimationFrame(this.id);
         } else {
            let that = this;
            let delta = t - this.previousTimeStep;
            if (delta > this.frameDuration) {
                this.nextStep(t);
                this.previousTimeStep = t;
                this.step++;
            }
            this.id = window.requestAnimationFrame(function (t) {
                that.animate(t);
            });
        }
    }
}
