.PHONY: clean rpm test

rpm:
	python setup.py bdist_rpm

clean:
	python setup.py clean

test:
	PYTHONPATH=src python -m unittest discover -s tests/ -v
