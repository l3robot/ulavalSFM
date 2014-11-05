Utilisateurs de Colosse, supercalculateur de l'Université Laval
--------------------------------------------------------------------------------------

#### Voilà quelques précisions pour installer ulavalSFM sur colosse :

###### Divers

- Créez un fichier .ulavalsfm dans $HOME où vous écrirez :

```Bash
#File used by ulavalSFM

RAP:<Votre RAP colosse ici>;
```

###### Module à charger/changer

- module add apps/cmake/2.8.12.1
- module add apps/git/1.8.5.3
- module add libs/mkl/11.1
- module add libs/opencv/2.4.9
- module add libs/ceres-solver/1.9.0

###### Installation

- Installer anaconda version Linux dans votre home https://store.continuum.io/cshop/anaconda/
- Pour que le programme soit plus rapide, installer ulavalSFM sur $SCRATCH

```Bash
cd $SCRATCH
git clone https://github.com/LERobot/ulavalSFM
cd <ulavalSFM>/
source install.sh -colosse
```

###### Utilisation

```Bash
cd <ulavalSFM>/bin
cp <chemin_vers_votre_dataset> <ulavalSFM>/bin
cd <dataset>
python ../bundler.py --no-parallel --verbose --number-cores <number_cores_u_want> --cluster --walltime <walltime_u_want>
```
Le programme exécutera les SIFTs et les appariements sur le cluster, attendra la fin de l'exécution et exécutera finalement bundlerSFM


