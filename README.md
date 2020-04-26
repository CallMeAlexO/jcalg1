# JCALG1

A python module to interface with Jeremy Collake's [JCALG1](https://github.com/jeremycollake/jcalg1) compression algorithm.

## Getting started

### Installation

Clone the github repo

```
git clone https://github.com/CallMeAlexO/jcalg1.git
cd jcalg1
```

Then just install

```
python setup.py install
```

### Usage

I planned to make JCALG1 as similar as possible to zlib, for consistancy. 
There are two functions: `compress` and `decompress`. 

```python
import jcalg1

text = """
Mary had a little lamb,
Little lamb, little lamb,
Mary had a little lamb
Whose fleece was white as snow.

And everywhere that Mary went,
Mary went, Mary went,
Everywhere that Mary went
The lamb was sure to go.""".encode('utf-8')

compressed = jcalg1.compress(text, level=9, skip_checksum=False)
decompressed = jcalg1.decompress(compressed)

print(decompressed == text)
```

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details

## Acknowledgments

* The algorithm's original author - Jeremy Collake, who gave me permission to share this on github. Thank you Jeremy!
