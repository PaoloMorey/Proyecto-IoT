conda-update:
	conda env update --prune -f environment.yml

pip-tools:
	# pip install pip-tools
	python -m pip install pip-tools
	pip-compile requirements/prod.in && pip-compile requirements/dev.in

# execute after pull this repo
	pip-sync requirements/prod.txt requirements/dev.txt
