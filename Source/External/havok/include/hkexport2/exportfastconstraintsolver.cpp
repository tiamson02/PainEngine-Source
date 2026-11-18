/* 
 * 
 * Confidential Information of Telekinesys Research Limited (t/a Havok).  This software contains code, techniques and know-how
 * which is confidential and proprietary to Havok.  Not for disclosure or distribution without prior written consent.
 * Havok Game Dynamics SDK (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. Use of this software is subject to the terms of an end user license agreement.
 * 
 */



#include <hkexport2/hkexport.h>

#include <hkexport2/exportconstraint.h>
#include <hkexport2/exportfastconstraintsolver.h>
#include <hkexport2/exportworld.h>

using namespace NS_ExportStream;

int ExportFastConstraintSolver::priv_CLASS_SerialNumber = 1; 

EXPORT_SINGLETON_IMPLEMENTATION(ExportFastConstraintSolverActionFactory);

ExportAction* ExportFastConstraintSolverActionFactory::CreateExportAction(Token &token, ExportSubspace *subspace, CheckLog& log)
{
	if (token=="FastConstraintSolver") return new ExportFastConstraintSolver(subspace);
	return HK_NULL;
}


// Constructor
ExportFastConstraintSolver::ExportFastConstraintSolver (ExportSubspace *subspace) : ExportAction (subspace)
{
	PROT_AutoName();
	priv_CLASS_SerialNumber++;
}

// Destructor
ExportFastConstraintSolver::~ExportFastConstraintSolver() 
{
	ExportConstraint_VectorIterator cons_it;
	for (cons_it=priv_constraints_vector.begin(); cons_it<priv_constraints_vector.end(); cons_it++)
		delete (*cons_it);
}

// Import
void ExportFastConstraintSolver::Import(InExportStream &xtream, CheckLog &log)
{
	xtream>>prot_buffer; SetName(prot_buffer); 
	log.BeginLevel(LOC("FAST CONSTRAINT SOLVER"),prot_name);

	xtream>>prot_token;

	while (prot_token=="BEGIN_CONSTRAINT")
	{
		xtream>>prot_token; 
		
		ExportConstraint *constraint= ExportConstraintRegistry::getInstance().CreateExportConstraint(prot_token, this, log);

		if (!constraint)
		{
			char buffer[200];
			hkString::sprintf(buffer,LOC("Import of constraint type: %s unsupported in Havok 2."),prot_token.GetString());
			PROT_WarningAndRecover( buffer, xtream, "END_CONSTRAINT", log, false );
			xtream >> prot_token;
		}
		else
		{
			constraint->Import(xtream, log);
			if (!constraint->GetHKConstraint())
			{
				delete constraint;
			}
			else
			{
				AddConstraint(constraint);
				if (constraint->GetIsBreakable())
				{
					GetSubspace()->GetWorld()->GetHKWorld()->addConstraint(constraint->GetHKBreakableConstraint());
				}
				else
				{
					GetSubspace()->GetWorld()->GetHKWorld()->addConstraint(constraint->GetHKConstraint());
				}
			}
			xtream>>prot_token;
		}
	}

	if (prot_token!="RB_COLLECTION") throw_exception("Unexpected token. RB_COLLECTION was expected");
	xtream>>prot_buffer; // this is the name of the rigid body collection from Havok 1 and can be safely ignored

	if (xtream.GetVersion() >= 1340)
	{
		float deactivation_threshold;
		xtream.TokenMustBe("DEACTIVATION_THRESHOLD"); xtream>>deactivation_threshold;
	}
	xtream.TokenMustBe("END_ACTION");
	log.EndLevel();
}

// Management of the constraints
ExportConstraint *ExportFastConstraintSolver::GetConstraint(char *name) const
{
	ExportConstraint_VectorIterator cons_it;
	for (cons_it=priv_constraints_vector.begin(); cons_it<priv_constraints_vector.end(); cons_it++)
	{
		if (!hkString::strCmp((*cons_it)->GetName(),name)) return *cons_it;
	}
	return HK_NULL;
}

/*
* Havok Game Dynamics SDK - PUBLIC RELEASE, BUILD(#20030622)
*
* (C) Copyright 1999/2003 Telekinesys Research Limited. All Rights Reserved. The Telekinesys
* Logo, Havok and the Havok buzzsaw logo are trademarks of Telekinesys
* Research Limited.  Title, ownership
* rights, and intellectual property rights in the Software remain in
* Telekinesys Research Limited and/or its suppliers.
*
* Use of this software is subject to and indicates acceptance of the End User licence
* Agreement for this product. A copy of the license is included with this software and
* is also available from info@havok.com.
*
*/ 
