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
    => ["B-ADJP", "B-ADVP", "B-CONJP" ...]
    model.save('ch.mod')
    => # saves the model as 'ch.mod'

Alternatively, you can pass in the training data as an array; the array
should contain one array for each sequence of training data.

    data = []
    data << ['Confidence NN B-NP', 'in IN B-PP', 'the DT B-NP', 'pound NN I-NP', '. . O']
    ...
    model = Wapiti.train(data, options)

You can consult the `Wapiti::Options` class for a list of supported
configuration options and algorithms:

    Wapiti::Options.attribute_names
    => [:algorithm, :check, :compact, :convergence_window, :development_data,
    :jobsize, :label, :max_iterations, :maxent, :pattern, :posterior, :rho1,
    :rho2, :score, :sparse, :stop_epsilon, :stop_window, :threads]
    Wapiti::Options.algorithms
    => ["l-bfgs", "sgd-l1", "bcd", "rprop", "rprop+", "rprop-", "auto"]

Use `#valid?` or `#validate` (which returns error messages) to make sure
your configuration is supported by Wapiti.

You can pass options either as an options hash or by adding a block to the
method invocation:

    model = Wapiti::Model.train(data) do |config|
      config.pattern = 'pattern.txt'
      threads = 4
    end

Before saving your model you can use `compact` to reduce the model's size:

    model.save 'm1.mod'
    => # m1.mod file size 1.8M
    model.compact
    model.save 'm2.mod'
    => # m2.mod file size 471K

### Loading existing Models

    model = Wapiti::Model.load('m1.mod')

### Labelling

By calling `#label` on a Model instance you can add labels to your sequence
data:

    model = Waiti.load('m2.mod')
    model.label('test.txt')
    => [[["Confidence NN B-NP", "B-NP"], ["in IN B-PP", "B-PP"] ... ]

The result is an array of sequence arrays; each sequence array consists of
the original token and feature string (when using test data, the final
feature is usually the expected label) and the label calculated by Wapiti.

As with training data, you can pass in data either by filename or as
a Ruby Array:

    model.label [['Confidence NN', 'in IN', 'the DT', 'pound NN', '. .']]
    => [[["Confidence NN", "B-NP"], ["in IN", "B-PP"], ["the DT", "B-NP"],
    ["pound NN", "I-NP"], [". .", "O"]]]

If you pass a block to `#label` Wapiti will yield each token and the
corresponding label:

    model.label [['Confidence NN', 'in IN', 'the DT', 'pound NN', '. .']] do |token, label|
      [token.downcase, label.downcase]
    end
    => [[["confidence nn", "b-np"], ["in in", "b-pp"], ["the dt", "b-np"],
    ["pound nn", "i-np"], [". .", "o"]]]


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
