function NumberDisplay(element, options) {
    this.options = Object.assign({
        maxDigits: 4,
        interval: 150, // ms
        order: '', // 'ltr' is default
        unit: 'V' // Unit to display
    }, options || {})

    this.DOMElements = {}
    this.construct(element)

    this.DOMElements.wrapper.addEventListener('transitionend', event => {
        if (event.pseudoElement === "::before" && event.propertyName == 'margin-top') {
            event.target.classList.remove('fuzzy')
        }
    })

    this.update()
}

NumberDisplay.prototype = {
    construct(element) {
        var wrapperElement = typeof element == 'string'
            ? document.querySelector(element)
            : element
                ? element
                : this.DOMElements.wrapper

        // Create the markup for 4 digits with a decimal place
        wrapperElement.innerHTML = `
            <div class="digit"><b data-value="0"></b></div>
            <div class="decimal"><b data-value="0"></b></div>
            <div class="digit"><b data-value="0"></b></div>
            <div class="digit"><b data-value="0"></b></div>
            <span class="unit">${this.options.unit}</span>
        `;

        this.DOMElements = {
            wrapper: wrapperElement,
            digits: wrapperElement.querySelectorAll('b'),
            unit: wrapperElement.querySelector('.unit')
        }
    },

    update(newValue) {
        var targetValue, className,
            settings = this.options,
            digitNodes = this.DOMElements.digits;

        this.currentValue = newValue || this.DOMElements.wrapper.dataset.value | 0

        if (!this.currentValue) return;

        // Convert value to a string with 4 digits and add leading zeros
        var formattedValue = this.currentValue.toFixed(3);
        var [intPart, decPart] = formattedValue.split('.');

        // Adjust for 4 digits with a decimal
        targetValue = [intPart[0], ...decPart.slice(0, 3)];

        if (settings.order == 'rtl') {
            targetValue = targetValue.reverse()
            digitNodes = [].slice.call(digitNodes).reverse()
        }

        // Loop through each digit element and update
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
    order: 'ltr',
    interval: 250,
    maxDigits: 4,
    unit: 'V'
})

var displayInterval = setInterval(randomUpdate, 4000)

function randomUpdate() {
    numberDisplay.update(+("" + Math.random()).substring(2, 6) / 1000)
}

function getRandomInteger(min, max) {
    return Math.floor(Math.random() * (max - min + 1) + min)
}

var controls = {
    visible: 0,
    CSSVariableTarget: document.querySelector('.numberDisplay'),
    sliders: [
        {
            label: 'Digit Count',
            type: 'range',
            value: 4,
            min: 1,
            max: 6,
            onChange: e => {
                clearInterval(displayInterval)
                numberDisplay.options.maxDigits = +e.target.value;
                numberDisplay.construct()
                displayInterval = setInterval(randomUpdate, 4000)
                randomUpdate()
            }
        },
        {
            label: 'Interval',
            type: 'range',
            value: 250,
            min: 50,
            max: 500,
            step: 50,
            onChange: e => numberDisplay.options.interval = +e.target.value
        },
        {
            cssVariable: ['hideAnimation'],
            label: 'Animate Left-To-Right',
            type: 'checkbox',
            checked: true,
            value: 'none',
            onChange: e => numberDisplay.options.order = e.target.checked ? "rtl" : ""
        }
    ]
}

var mySliders = new Knobs(controls)
