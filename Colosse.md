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
- module add libs/image_magick/6.8.9

###### Installation

- Installer anaconda version Linux dans votre home https://store.continuum.io/cshop/anaconda/
- TRÈS IMPORTANT : le chemin exécutable d'anaconda doit être $HOME/anaconda3/bin

```Bash
cd $SCRATCH
git clone https://github.com/LERobot/ulavalSFM
cd <ulavalSFM>/
source install.sh -colosse
```

###### Utilisation

```Bash
cd <votre_dataset>
bundler.py --no-parallel --verbose --number-cores <number_cores_u_want> --cluster --walltime <walltime_u_want>
```
Le programme exécutera les SIFTs et les appariements sur le cluster, finalement bundlerSFM sur colosse


