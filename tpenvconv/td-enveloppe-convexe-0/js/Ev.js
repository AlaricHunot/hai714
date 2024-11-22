class Ev {
    //un petit hack pour faire en sorte que la gestion des évènements se fasse avec "le bon this"
    static manage(listener, callback) {
        return function (e) {
            callback.call(listener, e);
        }
    }
}