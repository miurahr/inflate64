=========
ChangeLog
=========

All notable changes to this project will be documented in this file.

`Unreleased`_
=============

v1.0.0rc2_
==========

Changed
-------
* Update test runner for library developer (#11)
* Project status to be stable
* docs: update security policy


v1.0.0rc1_
==========

Added
-----
* Support python 3.12 (#9)

Fixed
-----
* Replace deprecated PyMem_Calloc with GIL free PyMem_RawCalloc
* Use PyMem_RawFree accordingly.

Changed
-------
* Bump cibuildwheel@2.16.2
* Update cibuildwheel configuration
* CI on ci.codeberg.org
* Minimum required Python verison to 3.8

v0.3.1_
=======

Fixed
-----
* Fix import error on python 3.7.

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
.. _Unreleased: https://codeberg.org/miurahr/inflate64/compare/v1.0.0rc2...HEAD
.. _v1.0.0rc2: https://codeberg.org/miurahr/inflate64/compare/v1.0.0rc1...v1.0.0rc2
.. _v1.0.0rc1: https://codeberg.org/miurahr/inflate64/compare/v0.3.1...v1.0.0rc1
.. _v0.3.1: https://codeberg.org/miurahr/inflate64/compare/v0.3.0...v0.3.1
.. _v0.3.0: https://codeberg.org/miurahr/inflate64/compare/v0.2.0...v0.3.0
.. _v0.2.0: https://codeberg.org/miurahr/inflate64/compare/v0.1.4...v0.2.0
.. _v0.1.4: https://codeberg.org/miurahr/inflate64/compare/v0.1.3...v0.1.4
.. _v0.1.3: https://codeberg.org/miurahr/inflate64/compare/v0.1.2...v0.1.3
.. _v0.1.2: https://codeberg.org/miurahr/inflate64/compare/v0.1.1...v0.1.2
.. _v0.1.1: https://codeberg.org/miurahr/inflate64/compare/v0.1.0...v0.1.1
