Wapiti-Ruby
===========

The Wapiti-Ruby gem provides a wicked fast linear-chain CRF
([Conditional Random Fields](http://en.wikipedia.org/wiki/Conditional_random_field))
API for sequence segmentation and labelling; it is based on the codebase of
Thomas Lavergne's awesome [wapiti](http://wapiti.limsi.fr/).


Requirements
------------

Wapiti-Ruby is written in C and Ruby and requires a compiler with C99
support (e.g., gcc); the gem has been confirmed to work with MRI 1.9, 1.8.7,
and Rubinius.


Quickstart
----------

### Installation

    $ [sudo] gem install wapiti

### Creating a Model

Using a pattern and training data stored in a file:

    model = Wapiti.train('train.txt', :pattern => 'pattern.txt')
		=> #<Wapiti::Model:0x0000010188f868>
		model.labels
		# => ["B-ADJP", "B-ADVP", "B-CONJP" ...]
		model.save('ch.mod')
		# => saves the model as 'ch.mod'

Alternatively, you can pass in the training data as an array; the array
should contain one array for each sequence of training data.


Citing
------

If you're using Wapiti-Ruby for research purposes, please use the following
citation of the original wapiti package:

    @article{lavergne2010practical,
      author    = {Lavergne, Thomas and Capp\'{e}, Olivier and Yvon, Fran\c{c}ois},
      title     = {Practical Very Large Scale {CRFs}},
      booktitle = {Proceedings the 48th Annual Meeting of the Association for
                  Computational Linguistics (ACL)},
      month     = {July},
      year      = {2010},
      location  = {Uppsala, Sweden},
      publisher = {Association for Computational Linguistics},
      pages     = {504--513},
      url       = {http://www.aclweb.org/anthology/P10-1052}
    }

If you're profiting from any of the Wapiti-Ruby specific features you are
welcome to also refer back to the
[Wapiti-Ruby homepage](http://github.com/inukshuk/wapiti-ruby/).


Contributing
------------

The Wapiti-Ruby source code is
[hosted on GitHub](http://github.com/inukshuk/wapiti-ruby/).
You can check out a copy of the latest code using Git:

    $ git clone https://github.com/inukshuk/wapiti-ruby.git

If you've found a bug or have a question, please open an issue on the
[Wapiti-Ruby issue tracker](http://github.com/inukshuk/wapiti-ruby/issues).
Or, for extra credit, clone the Wapiti-Ruby repository, write a failing
example, fix the bug and submit a pull request.


License
-------

Copyright 2011 Sylvester Keil. All rights reserved.

Copyright 2009-2011 CNRS. All rights reserved.

Wapiti-Ruby is distributed under a BSD-style license. See LICENSE for details.
