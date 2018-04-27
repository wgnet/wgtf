.pragma library

/*! \file Common utility functions
 */

/*! Returns the escaped text replacing all regular expression special characters
*/
if (typeof(RegExp.escape) === 'undefined') {
    RegExp.escape = function(text){
        return text.replace(/[-[\]{}()*+?.,\\^$|#]/g, "\\$&");
    };
}

/*! Compares strings the same way strcmp in C++ does string comparison, which is different than localeCompare.
*/
if (typeof(String.prototype.strcmp) === 'undefined') {
    String.prototype.strcmp = function(str) {
        return ((this == str) ? 0 : ((this > str) ? 1 : -1));
    };
}

/*! Replace all instances of the search string with the replacement string.
*/
if (typeof(String.prototype.replaceAll) === 'undefined') {
    String.prototype.replaceAll = function(search, replacement) {
        var target = this;
        return target.replace(new RegExp(RegExp.escape(search), 'g'), replacement);
    };
}

/*! Determines if the string ends with the specified string.
*/
if (typeof(String.prototype.endsWith) === 'undefined') {
    String.prototype.endsWith = function(search) {
        return this.indexOf(search) === this.length - search.length;
    };
}

/*! Determines if the string starts with the specified string.
*/
if (typeof(String.prototype.startsWith) === 'undefined') {
    String.prototype.startsWith = function(search) {
        return this.indexOf(search) === 0;
    };
}

/*! Searches the specified collection using a binary search algorithm.
    Assumes the provided collection is already sorted.
    Must specify an compare function that compares an item at a specified index with the search criteria

    Example:
    import "qrc:///WGControls/wg_utils.js" as WGUtils
    ...
    var haystack = ListModel{...}
    var needle = "needle"
    var compare = function(index){ return needle.strcmp(collection.item(index).value.path); }
    WGUtils.binarySearchCollection(haystack, haystack.count(), compare); 
*/
function binarySearchCollection(collection, length, compare) {
    var m = 0;
    var n = length - 1;
    while (m <= n) {
        var k = (n + m) >> 1;
        var cmp = compare(k);
        if (cmp > 0) {
            m = k + 1;
        } else if(cmp < 0) {
            n = k - 1;
        } else {
            return k;
        }
    }
    return -m - 1;
}

/*! Searches the specified array to find the specified element using a binary search algorithm.
    Assumes the provided array is already sorted.
    If the element searched for is not found the negative value of the insert location plus one is returned.

    Example:
    var numbers = [1,2,4,5];
    var element = 3;
    var index = binarySearch(numbers, element);
    if (index < 0) {
        // Insert the element into the sorted array
        numbers.splice(-index - 1, 0, element)
    } else {
        // Element found at index...
    }
*/
function binarySearch(ar, el) {
    return binarySearchCollection(ar, ar.length, function(index){ return el - ar[index]; });
}
