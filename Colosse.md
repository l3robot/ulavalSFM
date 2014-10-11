Utilisateurs de Colosse, supercalculateur de l'Université Laval
--------------------------------------------------------------------------------------

#### Voilà quelques précisions pour installer ulavalSFM sur colosse :

###### Divers

- Se connecter sur CentOS6 !!! Tout est fonctionnel sur CentOS6 puisqu'il y aura migration

###### Module à charger/changer

- module swap compilers/intel compilers/gcc/4.8
- module add apps/cmake/2.8.12.1
- module add apps/git/1.8.5.3
- module add libs/mkl/11.1

###### Installation

```Bash
git clone https://github.com/LERobot/ulavalSFM
cd <ulavalSFM>/
chmod +x install.sh
source install.sh -colosse
```

