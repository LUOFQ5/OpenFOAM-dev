/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     | Website:  https://openfoam.org
    \\  /    A nd           | Copyright (C) 2011-2023 OpenFOAM Foundation
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

Class
    Foam::engineValve

Description
    Foam::engineValve

SourceFiles
    engineValve.C

\*---------------------------------------------------------------------------*/

#ifndef engineValve_H
#define engineValve_H

#include "fvMeshMoversEngine.H"
#include "coordinateSystem.H"
#include "polyPatchDynamicID.H"
#include "Table.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

/*---------------------------------------------------------------------------*\
                         Class engineValve Declaration
\*---------------------------------------------------------------------------*/

class engineValve
{
    // Private Data

        //- Name of valve
        word name_;

        //- Reference to engine mesh mover
        const fvMeshMovers::engine& meshMover_;

        //- Coordinate system
        autoPtr<coordinateSystem> csPtr_;


        // Patch and zone names

            //- Valve bottom patch
            polyPatchDynamicID bottomPatch_;

            //- Valve poppet patch
            polyPatchDynamicID poppetPatch_;

            //- Valve stem patch
            polyPatchDynamicID stemPatch_;

            //- Valve curtain manifold patch
            polyPatchDynamicID curtainInPortPatch_;

            //- Valve curtain cylinder patch
            polyPatchDynamicID curtainInCylinderPatch_;

            //- Valve detach in cylinder patch
            polyPatchDynamicID detachInCylinderPatch_;

            //- Valve detach in port patch
            polyPatchDynamicID detachInPortPatch_;

            //- Faces to detach
            labelList detachFaces_;


        // Valve lift data

            //- Valve lift profile
            Function1s::Table<scalar> liftProfile_;

            //- Lift curve start angle
            scalar liftProfileStart_;

            //- Lift curve end angle
            scalar liftProfileEnd_;

            //- Minimum valve lift.  On this lift the valve is considered closed
            const scalar minLift_;


            // Valve layering data

                //- Min top layer thickness
                const scalar minTopLayer_;

                //- Max top layer thickness
                const scalar maxTopLayer_;

                //- Min bottom layer thickness
                const scalar minBottomLayer_;

                //- Max bottom layer thickness
                const scalar maxBottomLayer_;


            //- Valve diameter
            const scalar diameter_;


    // Private Member Functions

        //- Adjust crank angle to drop within the limits of the lift profile
        scalar adjustCrankAngle(const scalar theta) const;


public:

    // Constructors

        //- Construct from components
        engineValve
        (
            const word& name,
            const fvMeshMover& meshMover,
            const autoPtr<coordinateSystem>& valveCS,
            const word& bottomPatchName,
            const word& poppetPatchName,
            const word& stemPatchName,
            const word& curtainInPortPatchName,
            const word& curtainInCylinderPatchName,
            const word& detachInCylinderPatchName,
            const word& detachInPortPatchName,
            const labelList& detachFaces,
            const Function1s::Table<scalar>& liftProfile,
            const scalar minLift,
            const scalar minTopLayer,
            const scalar maxTopLayer,
            const scalar minBottomLayer,
            const scalar maxBottomLayer,
            const scalar diameter

        );

        //- Construct from dictionary
        engineValve
        (
            const word& name,
            const fvMeshMover& meshMover,
            const dictionary& dict
        );

        //- Disallow default bitwise copy construction
        engineValve(const engineValve&) = delete;


    // Member Functions

        //- Return name
        const word& name() const
        {
            return name_;
        }

        //- Return coordinate system
        const coordinateSystem& cs() const
        {
            return csPtr_();
        }

        //- Return lift profile
        const Function1s::Table<scalar>& liftProfile() const
        {
            return liftProfile_;
        }

        //- Return valve diameter
        scalar diameter() const
        {
            return diameter_;
        }


        // Valve patches

            //- Return ID of bottom patch
            const polyPatchDynamicID& bottomPatchIndex() const
            {
                return bottomPatch_;
            }

            //- Return ID of poppet patch
            const polyPatchDynamicID& poppetPatchIndex() const
            {
                return poppetPatch_;
            }

            //- Return ID of stem patch
            const polyPatchDynamicID& stemPatchIndex() const
            {
                return stemPatch_;
            }

            //- Return ID of curtain in cylinder patch
            const polyPatchDynamicID& curtainInCylinderPatchIndex() const
            {
                return curtainInCylinderPatch_;
            }

            //- Return ID of curtain in port patch
            const polyPatchDynamicID& curtainInPortPatchIndex() const
            {
                return curtainInPortPatch_;
            }


            //- Return ID of detach in cylinder patch
            const polyPatchDynamicID& detachInCylinderPatchIndex() const
            {
                return detachInCylinderPatch_;
            }

            //- Return ID of detach in port patch
            const polyPatchDynamicID& detachInPortPatchIndex() const
            {
                return detachInPortPatch_;
            }

            //- Return face labels of detach curtain
            const labelList& detachFaces() const
            {
                return detachFaces_;
            }


        // Valve layering thickness

            scalar minTopLayer() const
            {
                return minTopLayer_;
            }

            scalar maxTopLayer() const
            {
                return maxTopLayer_;
            }

            scalar minBottomLayer() const
            {
                return minBottomLayer_;
            }

            scalar maxBottomLayer() const
            {
                return maxBottomLayer_;
            }


        // Valve position and velocity

            //- Return valve lift given crank angle in degrees
            scalar lift(const scalar theta) const;

            //- Is the valve open?
            bool isOpen() const;

            //- Return current lift
            scalar curLift() const;

            //- Return valve velocity for current time-step
            scalar curVelocity() const;

            //- Return list of active patch labels for the valve head
            //  (stem is excluded)
            labelList movingPatchIndices() const;


        //- Write dictionary
        void writeDict(Ostream&) const;


    // Member Operators

        //- Disallow default bitwise assignment
        void operator=(const engineValve&) = delete;
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
