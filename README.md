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
Wapiti is written in C and Ruby and requires a compiler with C99 support;
it has been confirmed to work on Linux, macOS, and Windows.

Quickstart
----------

### Installation

    $ [sudo] gem install wapiti

### Creating a Model

You can run the following examples starting the ruby interpreter (irb or pry) inside spec/fixtures directory.
 
Using a pattern and training data stored in a file:

    require 'wapiti'
    model = Wapiti.train('chtrain.txt', pattern: 'chpattern.txt')
    #=> #<Wapiti::Model:0x0000010188f868>
    model.labels
    #=> ["B-ADJP", "B-ADVP", "B-CONJP" ...]
    model.save('ch.mod')
    #=> saves the model as 'ch.mod'

Alternatively, you can pass in the training data as a `Wapiti::Dataset`;
this class supports the default text format used by Wapiti as well as
additional formats (such as YAML or XML) and an API, to make it easier
to manage data sets used for input and training.

    options = {threads:3, pattern: 'chpattern.txt'} 
    
    data_text = Wapiti::Dataset.open('chtrain.txt',tagged:true)
    model2= Wapiti.train(data_text,options)
    model2.labels
    => ["B-ADJP", "B-ADVP", "B-CONJP" ...]

    options = {threads:3, pattern: 'chpattern_only_tag.txt'} 
    
    data_xml    = Wapiti::Dataset.open('chtrain.xml')
    #=> #<Wapiti::Dataset sequences={823}>
    model3   = Wapiti.train(data_xml, options)

You can consult the `Wapiti::Options.attribute_names` class for a list of
supported configuration options and `Wapiti::Options.algorithms` for
all supported algorithms:

Use `#valid?` or `#validate` (which returns error messages) to make sure
your configuration is supported by Wapiti.

Before saving your model you can use `compact` to reduce the model's size:

    model.save 'm1.mod'
    #=> m1.mod file size 1.8M
    model.compact
    model.save 'm2.mod'
    #=> m2.mod file size 471K


### Loading existing Models

    model = Wapiti.load('m1.mod')

### Labelling

By calling `#label` on a Model instance you can add labels to a dataset:

    model = Wapiti.load('ch.mod')
    input = Wapiti::Dataset.open('chtrain.txt',tagged:true)
    output = model.label(input)

The result is a new `Wapiti::Dataset` with the predicted labels for each
token. If your input data was already tagged, you can compare the input
and output datasets to evaluate your results:

    output - input
    # => new dataset of output sequences which are tagged differently than expected

If you pass a block to `#label` Wapiti will yield each token and the
corresponding label:

    model.label input do |token, label|
      [token.downcase, label.downcase]
    end

Note that if you set the *:score* option (either in the Model's `#options` or
when calling `#label`), the score for each label will be appended to
each token/label tuple as a floating point number or passed as a third
argument to the passed-in block.

    output_with_score = model.label input, score: true
    # => Dataset where each token will include a score
    output_with_score.first.map(&:score)
    # => [5.950832716249245, 8.870883529621942, ...]
    
### Statistics

By setting the *:check* option you can tell Wapiti to keep statistics during
the labelling phase (for the statistics to be meaningful you obviously need
to provide input data that is already labelled). Wapiti does not reset the
counters during consecutive calls to `#label` to allow you to collect
accumulative stats; however, you can reset the counters at any time, by calling
`#reset_counters`.

After calling `#label` with the *:check* options set and appropriately labelled
input, you can access the statistics via `#statistics` (the individual values
are also available through the associated attribute readers).

    model.label input, check: true
    model.stats
    => {:token=>{:count=>19172, :errors=>36, :rate=>0.18777383684539956},
        :sequence=>{:count=>823, :errors=>28, :rate=>3.402187120291616}}

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
Copyright 2011-2018 Sylvester Keil. All rights reserved.

Copyright 2009-2013 CNRS. All rights reserved.

Wapiti-Ruby is distributed under a BSD-style license. See LICENSE for details.
