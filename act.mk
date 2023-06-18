## act is a docker based CI tool that reuses github actions files.
## using if: ${{ env.ACT }}
## or    if: ${{ !env.ACT }}
## can be used too control the actions of a workflow

.ONESHELL:
docker-start:## 	start docker
	touch requirements.txt
	test -d .venv || $(PYTHON3) -m virtualenv .venv
	( \
	   source .venv/bin/activate; pip install -q -r requirements.txt; \
	);
	( \
	    while ! docker system info > /dev/null 2>&1; do\
	    echo 'Waiting for docker to start...';\
	    if [[ '$(OS)' == 'Linux' ]]; then\
	     systemctl restart docker.service;\
	    fi;\
	    if [[ '$(OS)' == 'Darwin' ]]; then\
	     type -P docker && open --background -a /./Applications/Docker.app/Contents/MacOS/Docker;\
	    fi;\
	sleep 1;\
	done\
	)

## --verbose -v
## --bind -b
## --reuse -r
## are useful flags

## caution!
## binding the repo to an act instance
## can have side effects if a matrix
## is running in parallel!
## additionally
## you can use the act -C <sub_folder> -W ...
## to change the context of the workflow

## Try:
## make ubuntu-matrix reuse=true bind=true
ubuntu-matrix:submodules docker-start### 	run act/github workflow
	@type -P act || echo -e "Install act local CI tool.\nTry:\napt install act or brew install act\n"
	@export $(cat ~/gh_token.txt) && act -v $(REUSE) $(BIND) -C $(PWD)  -W $(PWD)/.github/workflows/$@.yml

