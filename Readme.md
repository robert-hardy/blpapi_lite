# Bridge API

Try

    curl -XGET -G -d type="BDH" http://localhost:8080/ -H 'Content-type: application/json'

to see the reply for a BDH. At the moment it is like this:

    {"results": [
      {"contract_name": "stir_eur", "field": "mid_price", "value": "99.05", "month": 6, "source": "mock", "year": 2017, "date": "2016-01-01"},
      {"contract_name": "stir_eur", "field": "mid_price", "value": "99.1", "month": 6, "source": "mock", "year": 2017, "date": "2016-01-02"},
      {"contract_name": "stir_eur", "field": "mid_price", "value": "99.05", "month": 6, "source": "mock", "year": 2017, "date": "2016-01-03"}
    ]}

## License

(The MIT license)

Copyright (c) 2016 Robert Hardy

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the 'Software'), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
