#ifndef IV_LV5_RAILGUN_JSFUNCTION_H_
#define IV_LV5_RAILGUN_JSFUNCTION_H_
#include <iv/ustringpiece.h>
#include <iv/lv5/arguments.h>
#include <iv/lv5/jsenv.h>
#include <iv/lv5/jsscript.h>
#include <iv/lv5/jsfunction.h>
#include <iv/lv5/jsarguments.h>
#include <iv/lv5/railgun/fwd.h>
#include <iv/lv5/railgun/code.h>
#include <iv/lv5/railgun/jsscript.h>
#include <iv/lv5/railgun/context_fwd.h>
#include <iv/lv5/railgun/vm_fwd.h>
#include <iv/lv5/railgun/frame.h>
namespace iv {
namespace lv5 {
namespace railgun {

class JSVMFunction : public JSFunction {
 public:
  JSVal Call(Arguments* args, const JSVal& this_binding, Error* e) {
    args->set_this_binding(this_binding);
    const std::pair<JSVal, VM::State> ret =
        static_cast<Context*>(args->ctx())->vm()->Execute(*args, this, e);
    if (ret.second == VM::STATE_NORMAL) {
      return JSUndefined;
    } else {
      return ret.first;
    }
  }

  JSVal Construct(Arguments* args, Error* e) {
    Context* const ctx = static_cast<Context*>(args->ctx());
    JSObject* const obj = JSObject::New(ctx, code_->ConstructMap(ctx));
    const JSVal proto = Get(ctx, symbol::prototype(), IV_LV5_ERROR(e));
    if (proto.IsObject()) {
      obj->set_prototype(proto.object());
    }
    assert(args->IsConstructorCalled());
    return Call(args, obj, e);
  }

  JSEnv* scope() const {
    return env_;
  }

  Code* code() const {
    return code_;
  }

  static JSVMFunction* New(Context* ctx,
                           railgun::Code* code, JSEnv* env) {
    return new JSVMFunction(ctx, code, env);
  }

  void InstantiateBindings(Context* ctx, Frame* frame, Error* e) {
    // step 1
    JSVal this_value = frame->GetThis();
    if (!code_->strict()) {
      if (this_value.IsNullOrUndefined()) {
        this_value = ctx->global_obj();
      } else if (!this_value.IsObject()) {
        JSObject* const obj = this_value.ToObject(ctx, IV_LV5_ERROR_VOID(e));
        this_value = obj;
      }
    }
    frame->set_this_binding(this_value);
    JSDeclEnv* env = static_cast<JSDeclEnv*>(frame->variable_env());
    JSVal* args = frame->arguments_begin();
    const std::size_t arg_count = frame->argc_;
    for (Code::Decls::const_iterator it = code_->decls().begin(),
         last = code_->decls().end(); it != last; ++it) {
      const Code::Decl& decl = *it;
      const Symbol sym = std::get<0>(decl);
      const Code::DeclType type = std::get<1>(decl);
      if (type == Code::PARAM) {
        const std::size_t param = std::get<2>(decl);
        if (param >= arg_count) {
          env->CreateAndSetMutable(sym, std::get<3>(decl), JSUndefined);
        } else {
          env->CreateAndSetMutable(sym, std::get<3>(decl), args[param]);
        }
      } else if (type == Code::PARAM_LOCAL) {
        // initialize local value
        const std::size_t param = std::get<2>(decl);
        const std::size_t target = std::get<3>(decl);
        if (param >= arg_count) {
          frame->GetLocal()[target] = JSUndefined;
        } else {
          frame->GetLocal()[target] = args[param];
        }
      } else if (type == Code::FDECL) {
        env->CreateAndSetMutable(sym, std::get<3>(decl), JSUndefined);
      } else if (type == Code::ARGUMENTS || type == Code::ARGUMENTS_LOCAL) {
        JSObject* args_obj = NULL;
        if (!code_->strict()) {
          args_obj = JSNormalArguments::New(
              ctx, this,
              code_->params(),
              frame->arguments_rbegin(),
              frame->arguments_rend(), env,
              IV_LV5_ERROR_VOID(e));
        } else {
          args_obj = JSStrictArguments::New(
              ctx, this,
              frame->arguments_rbegin(),
              frame->arguments_rend(),
              IV_LV5_ERROR_VOID(e));
        }
        if (type == Code::ARGUMENTS) {
          if (code_->strict()) {
            env->CreateAndSetImmutable(sym, std::get<3>(decl), args_obj);
          } else {
            env->CreateAndSetMutable(sym, std::get<3>(decl), args_obj);
          }
        } else {
          // initialize local value
          const std::size_t target = std::get<3>(decl);
          frame->GetLocal()[target] = args_obj;
        }
      } else if (type == Code::VAR) {
        env->CreateAndSetMutable(sym, std::get<3>(decl), JSUndefined);
      } else if (type == Code::FEXPR) {
        env->CreateAndSetImmutable(sym, std::get<3>(decl), this);
      } else if (type == Code::FEXPR_LOCAL) {
        // initialize local value
        const std::size_t target = std::get<3>(decl);
        frame->GetLocal()[target] = this;
      }
    }
  }

  bool IsNativeFunction() const {
    return false;
  }

  bool IsBoundFunction() const {
    return false;
  }

  core::UStringPiece GetSource() const {
    const std::size_t start_pos = code_->block_begin_position();
    const std::size_t end_pos = code_->block_end_position();
    return code_->script()->SubString(start_pos, end_pos - start_pos);
  }

  bool IsStrict() const {
    return code_->strict();
  }

  void MarkChildren(radio::Core* core) {
    core->MarkCell(code_);
    core->MarkCell(env_);
  }

 private:
  JSVMFunction(Context* ctx,
               railgun::Code* code, JSEnv* env)
    : JSFunction(ctx),
      code_(code),
      env_(env) {
    Error e;
    DefineOwnProperty(
        ctx, symbol::length(),
        DataDescriptor(
            JSVal::UInt32(static_cast<uint32_t>(code->params().size())),
            ATTR::NONE),
        false, &e);
    // section 13.2 Creating Function Objects
    set_cls(JSFunction::GetClass());
    set_prototype(context::GetClassSlot(ctx, Class::Function).prototype);

    JSObject* const proto = JSObject::New(ctx);
    proto->DefineOwnProperty(
        ctx, symbol::constructor(),
        DataDescriptor(this, ATTR::W | ATTR::C),
        false, &e);
    DefineOwnProperty(
        ctx, symbol::prototype(),
        DataDescriptor(proto, ATTR::W),
        false, &e);
    if (code->HasName()) {
      DefineOwnProperty(
          ctx, context::Intern(ctx, "name"),
          DataDescriptor(JSString::New(ctx, code->name()), ATTR::NONE),
          false, &e);
    }
    if (code->strict()) {
      JSFunction* const throw_type_error = ctx->throw_type_error();
      DefineOwnProperty(ctx, symbol::caller(),
                        AccessorDescriptor(throw_type_error,
                                           throw_type_error,
                                           ATTR::NONE),
                        false, &e);
      DefineOwnProperty(ctx, symbol::arguments(),
                        AccessorDescriptor(throw_type_error,
                                           throw_type_error,
                                           ATTR::NONE),
                        false, &e);
    }
  }

  Code* code_;
  JSEnv* env_;
};

} } }  // namespace iv::lv5::railgun
#endif  // IV_LV5_RAILGUN_JSFUNCTION_H_
