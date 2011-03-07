
/*
 * Very simple utility function for creating a "class".
 */

// declare
define(['lingwo_dictionary/util/extendPrototype'],
    function (extendPrototype) {
        return function (props) {
            var cons = props['_constructor'];
            delete props['_constructor'];
            extendPrototype(cons, props);
            return cons;
        };
    }
);

