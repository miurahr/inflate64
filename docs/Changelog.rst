=========
ChangeLog
=========

All notable changes to this project will be documented in this file.

`Unreleased`_
=============

v0.3.0_
=======

Fixed
-----
* Deflater: Fix SIGSEGV when larger files.

Changed
-------
* Use length_code table for the range as same as original deflate
* Add l_code(length) macro.

v0.2.0_
=======

Added
-----
* Deflater compession class(#2)
* Docs: add technical note about enhanced deflate(#3)

Changed
-------
* Enhanced and enabled compression test cases
* Add internal check for window alloc

v0.1.4_
=======

* Move project forge to CodeBerg.org.
* Drop github actions scripts.
* Add azure-pipelines YAML files.

v0.1.3_
=======

* Add Python 3.11 beta wheels.

v0.1.2_
=======

* Actions: Fix sdist build command to use pyproject.toml

v0.1.1_
=======

* First Alpha
* Support decompression/inflation

.. History links
.. _Unreleased: https://codeberg.org/miurahr/inflate64/compare/v0.3.0...HEAD
.. _v0.3.0: https://codeberg.org/miurahr/inflate64/compare/v0.2.0...v0.3.0
.. _v0.2.0: https://codeberg.org/miurahr/inflate64/compare/v0.1.4...v0.2.0
.. _v0.1.4: https://codeberg.org/miurahr/inflate64/compare/v0.1.3...v0.1.4
.. _v0.1.3: https://codeberg.org/miurahr/inflate64/compare/v0.1.2...v0.1.3
.. _v0.1.2: https://codeberg.org/miurahr/inflate64/compare/v0.1.1...v0.1.2
.. _v0.1.1: https://codeberg.org/miurahr/inflate64/compare/v0.1.0...v0.1.1
