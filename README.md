# CLOUD-HEP-MINI


Para entrar al docker donde tenemos los root y los analizaers, hacemos:

```bash
docker run -it --name analisis_ul16v2   -v /hepdata/David/cms_open_data_work/CMSSW_10_6_30:/code   -u $(id -u):$(id -g)   cmsopendata/cmssw_10_6_30-slc7_amd64_gcc700 /bin/bash
Setting up CMSSW_10_6_30
```

Para copiar un archivo desde docker hasta mi disco:
(Se supone que el archivo esta en un contenedor de docker) y nosotros estamos en el servidor. Debemos averiguar primero cual es el contenedor, hacemos (docker ps) e identificamos el Id del contenedor, en este caso es 7423311b0c57.  

```bash
docker cp 7423311b0c57:/code/CMSSW_10_6_30/src/Analisis/MyAnalyzer/python/data_completa.root .
```

Esto copia el archivo del contenedor a el servidor.

Si queremos bajarlo a nuestra compu, usamos: sftp.
