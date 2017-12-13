Wapiti-Ruby
===========
The Wapiti-Ruby gem provides a wicked fast linear-chain CRF
([Conditional Random Fields](http://en.wikipedia.org/wiki/Conditional_random_field))
API for sequence segmentation and labelling; it is based on the
codebase of [wapiti](http://wapiti.limsi.fr/).

[![Linux Build Status](https://travis-ci.org/inukshuk/wapiti-ruby.svg?branch=master)](https://travis-ci.org/inukshuk/wapiti-ruby)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/12rtxe2o8p55g1w6/branch/master?svg=true)](https://ci.appveyor.com/project/inukshuk/wapiti-ruby/branch/master)
[![Coverage Status](https://coveralls.io/repos/github/inukshuk/wapiti-ruby/badge.svg?branch=master)](https://coveralls.io/github/inukshuk/wapiti-ruby?branch=master)

Requirements
------------
Wapiti is written in C and Ruby and requires a compiler with C99 support; it has
been confirmed to work on Linux, macOS, and Windows.

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
    => [:algorithm, :check, :compact, :convergence_window, :jobsize,
    :label, :max_iterations, :maxent, :pattern, :posterior, :rho1,
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

    model = Wapiti.load('m1.mod')

### Labelling

By calling `#label` on a Model instance you can add labels to your sequence
data:

    model = Wapiti.load('m2.mod')
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

Note that if you set the *:score* option (either in the Model's `#options` or
when calling `#label`), the score for each label will be appended to
each token/label tuple as a floating point number or passed as a third
argument to the passed-in block.

    model.label [['Confidence NN']], :score => true
    => [[["Confidence NN", "B-NP", 4.642034838737357]]]

Similarly, if you set the *:nbest* option to a value greater than one, Wapiti
will append more label and, optionally, score values to each tuple.

    model.label [['Confidence NN']], :score => true, :nbest => 3, :skip_tokens => true
    => [[["B-NP", 4.642034838737357, "B-VP", 1.7040256847206927, "B-ADJP", 0.7636429298060177]]]

Note how we also suppressed the output of the token string using the
*:skip_tokens* option.

### Statistics

By setting the *:check* option you can tell Wapiti to keep statistics during
the labelling phase (for the statistics to be meaningful you obviously need
to provide input data that is already labelled). Wapiti does not reset the
counters during consecutive calls to `#label` to allow you to collect
accumulative date; however, you can reset the counters at any time, by calling
`#reset_counters`.

After calling `#label` with the *:check* options set and appropriately labelled
input, you can access the statistics via `#statistics` (the individual values
are also available through the associated attribute readers).

    model.label 'test.txt', :check => true
    => {:token=>{:count=>1896, :errors=>137, :rate=>7.225738396624472},
    :sequence=>{:count=>77, :errors=>50, :rate=>64.93506493506494}}

For convenience, you can also use the `#check` method, which
will reset the counters, check your input, and return the stats.

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
Copyright 2011-2017 Sylvester Keil. All rights reserved.

Copyright 2009-2013 CNRS. All rights reserved.

Wapiti-Ruby is distributed under a BSD-style license. See LICENSE for details.
