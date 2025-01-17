/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2021-2023 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

\*---------------------------------------------------------------------------*/

#include "singleComponentPhaseChangeBase.H"
#include "basicThermo.H"
#include "multicomponentThermo.H"
#include "addToRunTimeSelectionTable.H"

// * * * * * * * * * * * * * Static Member Functions * * * * * * * * * * * * //

namespace Foam
{
namespace fv
{
    defineTypeNameAndDebug(singleComponentPhaseChangeBase, 0);
}
}


// * * * * * * * * * * * * * Private Member Functions  * * * * * * * * * * * //

void Foam::fv::singleComponentPhaseChangeBase::readCoeffs()
{
    if
    (
        (specieThermos().valid().first() || specieThermos().valid().second())
     && specie_ != coeffs().lookup<word>("specie")
    )
    {
        FatalIOErrorInFunction(coeffs())
            << "Cannot change the specie of a " << typeName << " model "
            << "at run time" << exit(FatalIOError);
    }

    energySemiImplicit_ =
        coeffs().lookup<bool>("energySemiImplicit");
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

Foam::fv::singleComponentPhaseChangeBase::singleComponentPhaseChangeBase
(
    const word& name,
    const word& modelType,
    const fvMesh& mesh,
    const dictionary& dict,
    const Pair<bool> specieThermosRequired
)
:
    phaseChangeBase(name, modelType, mesh, dict, specieThermosRequired),
    specie_
    (
        specieThermos().valid().first() || specieThermos().valid().second()
      ? coeffs().lookup<word>("specie")
      : word::null
    ),
    specieis_
    (
        specieThermos().valid().first()
      ? specieThermos().first().species()[specie_]
      : -1,
        specieThermos().valid().second()
      ? specieThermos().second().species()[specie_]
      : -1
    ),
    energySemiImplicit_(false)
{
    readCoeffs();
}


// * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * * //

void Foam::fv::singleComponentPhaseChangeBase::addSup
(
    const volScalarField& alpha,
    const volScalarField& rho,
    const volScalarField& heOrYi,
    fvMatrix<scalar>& eqn
) const
{
    const label i = index(phaseNames(), alpha.group());
    const label s = sign(phaseNames(), alpha.group());

    // Energy equation
    if (index(heNames(), heOrYi.name()) != -1)
    {
        const volScalarField& p = this->p();
        const volScalarField Tchange(vifToVf(this->Tchange()));

        const volScalarField::Internal mDotIn(s*mDot());

        // Absolute enthalpies at the interface
        Pair<tmp<volScalarField::Internal>> has;
        for (label j = 0; j < 2; ++ j)
        {
            has[j] =
                specieThermos().valid()[j]
              ? vfToVif(specieThermos()[j].hai(specieis_[j], p, Tchange))
              : vfToVif(thermos()[j].ha(p, Tchange));
        }

        // Direct transfer of energy due to mass transfer
        if (energySemiImplicit_)
        {
            eqn += -fvm::SuSp(-mDotIn, heOrYi) + mDotIn*(has[i]() - heOrYi);
        }
        else
        {
            eqn += mDotIn*has[i]();
        }

        // Latent heat of phase change
        eqn +=
            (i == 0 ? Lfraction() - 1 : Lfraction())
           *mDotIn*(has.second() - has.first());
    }
    // Mass fraction equation
    else if
    (
        specieThermos().valid()[i]
     && specieThermos()[i].containsSpecie(heOrYi.member())
    )
    {
        // The transferring specie
        if (heOrYi.member() == specie())
        {
            eqn += s*mDot();
        }
        // A non-transferring specie. Do nothing.
        else
        {}
    }
    // Something else. Fall back.
    else
    {
        massTransferBase::addSup(alpha, rho, heOrYi, eqn);
    }
}


bool Foam::fv::singleComponentPhaseChangeBase::read(const dictionary& dict)
{
    if (phaseChangeBase::read(dict))
    {
        readCoeffs();
        return true;
    }
    else
    {
        return false;
    }
}


// ************************************************************************* //
