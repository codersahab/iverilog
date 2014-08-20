/*
 * Copyright (c) 2011 Stephen Williams (steve@icarus.com)
 *
 *    This source code is free software; you can redistribute it
 *    and/or modify it in source code form under the terms of the GNU
 *    General Public License as published by the Free Software
 *    Foundation; either version 2 of the License, or (at your option)
 *    any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program; if not, write to the Free Software
 *    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

# include  "vsignal.h"
# include  "expression.h"
# include  "vtype.h"
# include  <iostream>

using namespace std;

SigVarBase::SigVarBase(perm_string nam, const VType*typ, Expression*exp)
: name_(nam), type_(typ), init_expr_(exp), refcnt_sequ_(0)
{
}

SigVarBase::~SigVarBase()
{
}

void SigVarBase::elaborate_init_expr(Entity*ent, Architecture*arc)
{
    if(init_expr_) {
      // convert the initializing string to bitstring if applicable
      const ExpString*string = dynamic_cast<const ExpString*>(init_expr_);
      if(string) {
        const std::vector<char>& val = string->get_value();
        char buf[val.size() + 1];
        std::copy(val.begin(), val.end(), buf);
        buf[val.size()] = 0;

        ExpBitstring*bitstring = new ExpBitstring(buf);
        delete init_expr_;
        init_expr_ = bitstring;
      }
      else {
        ExpAggregate*aggr = dynamic_cast<ExpAggregate*>(init_expr_);
        if(aggr) {
          aggr->elaborate_expr(ent, arc, peek_type());
        }
      }
    }
}

void SigVarBase::type_elaborate_(VType::decl_t&decl)
{
      decl.type = type_;
}

int Signal::emit(ostream&out, Entity*ent, Architecture*arc)
{
      int errors = 0;

      VType::decl_t decl;
      type_elaborate_(decl);
      if (peek_refcnt_sequ_() > 0)
	    decl.reg_flag = true;
      errors += decl.emit(out, peek_name_());

      Expression*init_expr = peek_init_expr();
      if (init_expr) {
	    out << " = ";
	    init_expr->emit(out, ent, arc);
      }
      out << ";" << endl;
      return errors;
}

int Variable::emit(ostream&out, Entity*, Architecture*)
{
      int errors = 0;

      VType::decl_t decl;
      type_elaborate_(decl);
      if (peek_refcnt_sequ_() > 0)
	    decl.reg_flag = true;
      errors += decl.emit(out, peek_name_());
      out << ";" << endl;
      return errors;
}
