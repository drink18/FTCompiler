#ifndef ENV_H
#define ENV_H

#include "../ast/ast-id.h"
#include "type.h"

#define Id_t AstId_t

typedef struct
{
  Type_t type;
  AstId_t fresh;
} *Env_Binding_t;

Env_Binding_t Env_Binding_new (Type_t, AstId_t);

/* type env:
 *   Id_t -> Type_t
 */
void Tenv_init ();
void Tenv_insert (Id_t id, Type_t type);
Env_Binding_t Tenv_lookup (Id_t);
Env_Binding_t Tenv_lookupMustExist (Id_t);
/* type Genv:
 *   Id_t -> Type_t
 *   store global variable
 */


void Genv_init();
void Genv_insert (Id_t id, Type_t type);
Env_Binding_t Genv_lookup (Id_t);
Env_Binding_t Genv_lookupMustExist (Id_t);


/* variable env: 
 *   Stack<Hash<Id_t, Type_t>> 
 */
void Venv_init ();
Id_t Venv_insert (Id_t id, Type_t type);
Id_t Venv_insert_fresh(Id_t id, Type_t ty,Id_t fresh);

Env_Binding_t Venv_lookup (Id_t);
Env_Binding_t Venv_lookupMustExist (Id_t);
void Venv_enterScope ();
void Venv_exitScope ();

/* struct field env:
 *   Id_t * Id -> Id_t
 */
void Senv_init ();
void Senv_insert (Id_t sid, Id_t fid);
Env_Binding_t Senv_lookup (Id_t sid, Id_t fid);


#undef Id_t

#endif
