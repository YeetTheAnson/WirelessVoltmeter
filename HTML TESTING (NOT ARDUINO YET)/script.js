// Global variable to hold the fixed value to be displayed
const fixedValue = 5678; // Set the fixed value you want to display

function NumberDisplay(element, options) {
    this.options = Object.assign({
        maxDigits: 4, // Set to 4 digits
        interval: 150, // ms
        order: '' // 'ltr' is default
    }, options || {})

    this.DOMElements = {}
    this.construct(element)

    this.DOMElements.wrapper.addEventListener('transitionend', event => {
        if (event.pseudoElement === "::before" && event.propertyName == 'margin-top') {
            event.target.classList.remove('fuzzy')
        }
    })

    this.update(fixedValue) // Initialize with the fixed value
}

NumberDisplay.prototype = {
    construct(element) {
        var wrapperElement = typeof element == 'string'
            ? document.querySelector(element)
            : element
                ? element
                : this.DOMElements.wrapper

        wrapperElement.innerHTML = Array(this.options.maxDigits)
            .fill('<div><b data-value="0"></b></div>').join('');

        this.DOMElements = {
            wrapper: wrapperElement,
            digitElements: wrapperElement.querySelectorAll('b')
        }
    },

    update(newValue) {
        var targetValue,
            settings = this.options,
            digitNodes = this.DOMElements.digitElements;

        this.currentValue = newValue || this.DOMElements.wrapper.dataset.value | 0

        if (!this.currentValue) return;

        targetValue = (this.currentValue + '').padStart(settings.maxDigits, '0').split('')

        if (settings.order == 'rtl') {
            targetValue = targetValue.reverse()
            digitNodes = [].slice.call(digitNodes).reverse()
        }

        digitNodes.forEach(function (node, index) {
            if (+node.dataset.value != targetValue[index] && targetValue[index] >= 0)
                setTimeout(function (i) {
                    var difference = Math.abs(targetValue[i] - +node.dataset.value);
                    node.dataset.value = targetValue[i]
                    if (difference > 3)
                        node.className = 'fuzzy';
                }, index * settings.interval, index)
        })
    }
}

var numberDisplay = new NumberDisplay('.numberDisplay', {
    order: 'ltr', // Change direction if needed
    interval: 250,
    maxDigits: 4 // Set to 4 digits
})

// No randomization, just a fixed displayValue
// You can call `numberDisplay.update(fixedValue);` if you need to change the value programmatically

