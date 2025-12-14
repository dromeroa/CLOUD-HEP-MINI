# CLOUD-HEP-MINI


Para entrar al docker donde tenemos los root y los analizaers, hacemos:

```bash
docker run -it --name analisis_ul16v2   -v /hepdata/David/cms_open_data_work/CMSSW_10_6_30:/code   -u $(id -u):$(id -g)   cmsopendata/cmssw_10_6_30-slc7_amd64_gcc700 /bin/bash
Setting up CMSSW_10_6_30
```


