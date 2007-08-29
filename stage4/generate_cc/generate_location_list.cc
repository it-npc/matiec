/*
 * (c) 2007 Mario de Sousa and Laurent Bessard
 *
 * Offered to the public under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details.
 *
 * This code is made available on the understanding that it will not be
 * used in safety-critical situations without a full and competent review.
 */

/*
 * An IEC 61131-3 IL and ST compiler.
 *
 * Based on the
 * FINAL DRAFT - IEC 61131-3, 2nd Ed. (2001-12-10)
 *
 */

/*
 * This is one of the versions available for the 4th stage.
 *
 * This 4th stage generates a c++ source program equivalent
 * to the IL and ST code.
 */

//#include <stdio.h>  /* required for NULL */
//#include <string>
//#include <iostream>

//#include "../../util/symtable.hh"









/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
/***********************************************************************/

class generate_location_list_c: protected iterator_visitor_c {

  protected:
    stage4out_c &s4o;

  private:
    symbol_c *current_var_type_symbol;
    generate_cc_base_c *generate_cc_base;
    search_base_type_c search_base_type;
    
  public:
    generate_location_list_c(stage4out_c *s4o_ptr): s4o(*s4o_ptr) {
      generate_cc_base = new generate_cc_base_c(s4o_ptr);
      current_var_type_symbol = NULL;
    }
    ~generate_location_list_c(void) {
      delete generate_cc_base;
    }

    bool test_location_type(symbol_c *direct_variable) {
      
      token_c *location = dynamic_cast<token_c *>(direct_variable);
      
      if (location == NULL)
        /* invalid identifiers... */
        return false;
      
      switch (location->value[2]) {
        case 'X': // bit
          if (typeid(*current_var_type_symbol) == typeid(bool_type_name_c)) return true;
          break;
        case 'B': // Byte, 8 bits
          if (typeid(*current_var_type_symbol) == typeid(sint_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(usint_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(string_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(byte_type_name_c)) return true;
          break;
        case 'W': // Word, 16 bits
          if (typeid(*current_var_type_symbol) == typeid(int_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(uint_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(word_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(wstring_type_name_c)) return true;
          break;
        case 'D': // Double, 32 bits
          if (typeid(*current_var_type_symbol) == typeid(dint_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(udint_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(real_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(dword_type_name_c)) return true;
          break;
        case 'L': // Long, 64 bits
          if (typeid(*current_var_type_symbol) == typeid(lint_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(ulint_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(lreal_type_name_c)) return true;
          if (typeid(*current_var_type_symbol) == typeid(lword_type_name_c)) return true;
          break;
        default:
          if (typeid(*current_var_type_symbol) == typeid(bool_type_name_c)) return true;
      }
      return false;
    }

/********************************************/
/* B.1.4.1   Directly Represented Variables */
/********************************************/

    void *visit(direct_variable_c *symbol) {
      s4o.print("__LOCATED_VAR(");
      current_var_type_symbol->accept(*generate_cc_base);
      s4o.print(",");
      /* Do not use print_token() as it will change everything into uppercase */
      s4o.printlocation((symbol->value)+1);
      s4o.print(",");
      s4o.printlocation_comasep((symbol->value)+1);
      s4o.print(")\n");
      return NULL;
    }


/********************************************/
/* B.1.4.3   Declaration and initilization  */
/********************************************/

/*  [variable_name] location ':' located_var_spec_init */
/* variable_name -> may be NULL ! */
//SYM_REF4(located_var_decl_c, variable_name, location, located_var_spec_init, unused)
    void *visit(located_var_decl_c *symbol) {
        current_var_type_symbol = spec_init_sperator_c::get_spec(symbol->located_var_spec_init);
        if (current_var_type_symbol == NULL)
          ERROR;
        
        current_var_type_symbol = (symbol_c *)(current_var_type_symbol->accept(search_base_type));
        if (current_var_type_symbol == NULL)
          ERROR;
        
        symbol->location->accept(*this);
        
        current_var_type_symbol = NULL;
        return NULL;
    }

/*| global_var_spec ':' [located_var_spec_init|function_block_type_name] */
/* type_specification ->may be NULL ! */
//SYM_REF2(global_var_decl_c, global_var_spec, type_specification)
    void *visit(global_var_decl_c *symbol) {
        current_var_type_symbol = spec_init_sperator_c::get_spec(symbol->type_specification);
        if (current_var_type_symbol == NULL)
          ERROR;
        
        current_var_type_symbol = (symbol_c *)(current_var_type_symbol->accept(search_base_type));
        if (current_var_type_symbol == NULL)
          ERROR;
        
        symbol->global_var_spec->accept(*this);
        
        current_var_type_symbol = NULL;
        return NULL;
    }

/*  AT direct_variable */
//SYM_REF2(location_c, direct_variable, unused)
    void *visit(location_c *symbol) {
      if (test_location_type(symbol->direct_variable))
        symbol->direct_variable->accept(*this); 
      else
        ERROR;
      return NULL;
    }

}; /* generate_location_list_c */
