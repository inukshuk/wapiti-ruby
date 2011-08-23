Wapiti
======

The Wapiti Ruby gem provides a wicked fast linear-chain CRF
([Conditional Random Fields](http://en.wikipedia.org/wiki/Conditional_random_field))
API for sequence segmentation and labeling; it is based on the codebase of
[Thomas Lavergne's awesome C implementation](http://wapiti.limsi.fr/).

Requirements
------------

You will need a compiler with C99 support (e.g., gcc) to install Wapiti.

The Ruby extension is built for the Ruby 1.9 API. Ruby 1.8 and Rubinius are
not currently supported.

Quickstart
----------

    $ irb
    > require 'wapiti'
    > Wapiti.train do |config|
    >   config.pattern = 'pattern file'
    >   config.input = 'training data'
    >   config.output = 'model file'
    > end
 
