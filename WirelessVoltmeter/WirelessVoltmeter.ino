#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// Replace with your ssid and password
const char* ssid = "SSID";
const char* password = "Password";

// Create an instance of the server
ESP8266WebServer server(80);

// convert analog to voltage 
String getVoltage() {
  int analogValue = analogRead(A0);
  float voltage = analogValue * (3.3 / 1023.0); 
  char buffer[10];
  dtostrf(voltage, 1, 3, buffer); // convert float to string with 3 decimal places
  return String(buffer);
}

// handle the root request
void handleRoot() {
  String voltage = getVoltage();
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Voltage Display</title>
    <style>
        :root {
            --digitHeight: 80px;
            --transitionSpeed: 0.4s;
            --primaryColor: #ff6f61;
            --fontColor: #ffffff;
        }

        @import url('https://fonts.googleapis.com/css?family=Roboto+Condensed&display=swap');

        html, body {
            height: 100%;
            margin: 0;
        }

        body {
            display: flex;
            align-items: center;
            justify-content: center;
            background: var(--primaryColor);
            font-family: 'Roboto Condensed', Arial;
            perspective: 1200px;
        }

        .numberDisplay {
            display: inline-block;
            height: var(--digitHeight);
            line-height: var(--digitHeight);
            text-shadow: 0 0 2px #000;
            font-weight: bold;
            white-space: normal;
            font-size: calc(var(--digitHeight) / 1.3);
            overflow: hidden;
            padding: 0.5em;
            text-align: center;
            border-top: 1px solid rgba(0, 0, 0, 0.1);
            border-left: 0.5px solid rgba(0, 0, 0, 0.05);
            border-radius: 14px;
            background: linear-gradient(330deg, hsl(10, 80%, 60%), hsl(10, 80%, 40%));
            box-shadow: -15px -15px 50px hsl(10, 80%, 35%), 15px 15px 50px hsl(10, 80%, 70%);
        }

        .numberDisplay > div {
            display: inline-block;
            vertical-align: top;
            height: 100%;
        }

        .numberDisplay > div > b {
            display: inline-block;
            width: calc(var(--digitHeight) / 1.5);
            height: 100%;
            margin: 0 0.1em;
            border-radius: 6px;
            text-align: center;
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3);
            box-shadow: 1px 1px 2px rgba(0, 0, 0, 0.1) inset, -1px -1px 1px rgba(255, 255, 255, 0.15) inset;
            overflow: hidden;
            background: linear-gradient(-30deg, hsl(10, 80%, 65%), hsl(10, 80%, 30%));
            color: var(--fontColor);
        }

        .numberDisplay > div > b::before {
            content: ' 0 1 2 3 4 5 6 7 8 9 ';
            display: block;
            word-break: break-all;
            word-break: break-word;
            transition: var(--transitionSpeed) cubic-bezier(0.7, 0.3, 0.6, 1), text-shadow 100ms;
        }

        .numberDisplay > div > b.fuzzy {
            opacity: 0.8;
            text-shadow: 1px 1px 2px rgba(0, 0, 0, 0.3), 0 0.1em 1px rgba(255, 255, 255, 0.5), 0 0.3em 2px rgba(255, 255, 255, 0.25), 0 -0.1em 1px rgba(255, 255, 255, 0.5), 0 -0.3em 2px rgba(255, 255, 255, 0.25);
        }

        @media (max-width: 768px) {
            .numberDisplay {
                font-size: calc(var(--digitHeight) / 2.5);
            }
        }

        .numberDisplay > div:nth-last-child(3n):not(:first-child)::before {
            content: ".";
            display: inline;
            font-size: 1em;
            opacity: 0.5;
            color: black;
        }

        .numberDisplay > div > b[data-value="1"]::before { margin-top: calc(-1 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="2"]::before { margin-top: calc(-2 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="3"]::before { margin-top: calc(-3 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="4"]::before { margin-top: calc(-4 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="5"]::before { margin-top: calc(-5 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="6"]::before { margin-top: calc(-6 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="7"]::before { margin-top: calc(-7 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="8"]::before { margin-top: calc(-8 * var(--digitHeight)); }
        .numberDisplay > div > b[data-value="9"]::before { margin-top: calc(-9 * var(--digitHeight)); }
    </style>
</head>
<body>
    <div class='numberDisplay'></div>
    <script>
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

        var fixedValue = 0; // Placeholder for the fixed value

        var numberDisplay = new NumberDisplay('.numberDisplay', {
            order: 'ltr', // Change direction if needed
            interval: 250,
            maxDigits: 4 // Set to 4 digits
        });

        function updateValue() {
            fetch('/voltage')
                .then(response => response.text())
                .then(text => {
                    fixedValue = parseFloat(text);
                    numberDisplay.update(Math.round(fixedValue * 1000));
                });
        }

        setInterval(updateValue, 250); // Update every 250ms
    </script>
</body>
</html>
)rawliteral";

  server.send(200, "text/html", html);
}

// handle voltage requests
void handleVoltage() {
  server.send(200, "text/plain", getVoltage());
}

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP().toString());


  server.on("/", handleRoot);
  server.on("/voltage", handleVoltage);

  server.begin();
}

void loop() {
  server.handleClient();
}
