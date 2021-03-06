#ifndef C8_AST_HPP
#define C8_AST_HPP

#include <lingo/symbol.hpp>
#include <lingo/token.hpp>

#include <memory>
#include <vector>

namespace c8 {
namespace ast {

class Node;

template <class T = Node>
using NodePtr = std::unique_ptr<T>;
template <class T>
using NodeVector = std::vector<NodePtr<T>>;

// Forward declare node types
class Node;
struct Expression;
struct Statement;
struct BlockStatement;
struct Identifier;
struct Function;
struct FunctionDeclaration;
struct Literal;
struct BinaryExpression;
struct ReturnStatement;
struct ExpressionStatement;
struct UnaryExpression;
struct UpdateExpression;
struct IfStatement;
struct ForStatement;
struct Declaration;
struct VariableDeclaration;

class Visitor {
 public:
  virtual void Visit(FunctionDeclaration *node) = 0;
  virtual void Visit(BlockStatement *node) = 0;
  // virtual void Visit(Expression *node)

  virtual void Visit(ReturnStatement *node) = 0;
  virtual void Visit(ExpressionStatement *node) = 0;
  virtual void Visit(IfStatement *node) = 0;
  virtual void Visit(ForStatement *node) = 0;

  // Expression visitors
  virtual void Visit(BinaryExpression *node) = 0;
  virtual void Visit(UnaryExpression *node) = 0;
  virtual void Visit(UpdateExpression *node) = 0;
  virtual void Visit(Literal *node) = 0;
  virtual void Visit(Identifier *node) = 0;
  // virtual void Visit
};

class Node {
 public:
  virtual ~Node() {}
  // virtual void Accept(Visitor *v) = 0;
  enum NodeKind {
    Kind_STATEMENT_START,

    Kind_BlockStatement,
    Kind_ReturnStatement,
    Kind_ExpressionStatement,
    Kind_IfStatement,
    Kind_ForStatement,

    // DeclaratoionStart?
    Kind_DEVLARATION_START,
    Kind_FunctionDeclaration,
    Kind_VariableDeclaration,
    Kind_DECLARATION_END,
    Kind_STATEMENT_END = Kind_DECLARATION_END,

    Kind_EXPRESSION_START,
    Kind_Identifier,
    Kind_Literal,
    Kind_UpdateExpression,
    Kind_UnaryExpression,
    Kind_BinaryExpression,
    Kind_MemberExpression,
    Kind_EXPRESSION_END,

    Kind_VariableDeclarator
  };

  NodeKind Kind() const { return kind_; }

 protected:
  Node(NodeKind kind) : kind_(kind) {}

 private:
  const NodeKind kind_;
};

struct Expression : public Node {
  static bool classof(const Node *node) {
    return (node->Kind() > Kind_EXPRESSION_START) &&
           (node->Kind() < Kind_EXPRESSION_END);
  }

 protected:
  using Node::Node;
};

struct UpdateExpression : public Expression {
  NodePtr<Expression> argument;
  lingo::Token oper;
  bool prefix;

  static bool classof(const Node *node) {
    return node->Kind() == Kind_UpdateExpression;
  }

  // TODO deprecated
  UpdateExpression() : Expression(Kind_UpdateExpression){};
  UpdateExpression(NodePtr<Expression> &&arg, const lingo::Token &op, bool pfx)
      : Expression(Kind_UpdateExpression),
        argument(std::move(arg)),
        oper(op),
        prefix(pfx) {}
};

struct UnaryExpression : public Expression {
  NodePtr<Expression> argument;
  lingo::Token oper;

  static bool classof(const Node *node) {
    return node->Kind() == Kind_UnaryExpression;
  }
  UnaryExpression(lingo::Token oper_, NodePtr<Expression> &&arg)
      : Expression(Kind_UnaryExpression),
        argument(std::move(arg)),
        oper(oper_) {}
};

struct BinaryExpression : public Expression {
  NodePtr<Expression> left;
  NodePtr<Expression> right;
  lingo::Token oper;

  static bool classof(const Node *node) {
    return node->Kind() == Kind_BinaryExpression;
  }

  BinaryExpression(NodePtr<Expression> &&left_, NodePtr<Expression> &&right_,
                   lingo::Token oper_)
      : Expression(Kind_BinaryExpression),
        left(std::move(left_)),
        right(std::move(right_)),
        oper(oper_) {}
};

struct MemberExpression : public Expression {
  NodePtr<Expression> object;
  NodePtr<Expression> property;
  bool computed;

  static bool classof(const Node *node) {
    return node->Kind() == Kind_MemberExpression;
  }

  MemberExpression(NodePtr<Expression> &&object_,
                   NodePtr<Expression> &&property_, bool computed_)
      : Expression(Kind_MemberExpression),
        object(std::move(object_)),
        property(std::move(property_)),
        computed(computed_) {}
};

struct Statement : public Node {
  static bool classof(const Node *node) {
    return (node->Kind() > Kind_STATEMENT_START) &&
           (node->Kind() < Kind_STATEMENT_END);
  }

 protected:
  using Node::Node;
};

struct Declaration : public Statement {
 protected:
  using Statement::Statement;
};

struct VariableDeclarator : public Node {
  NodePtr<Identifier> id;
  NodePtr<Expression> init;

  VariableDeclarator(NodePtr<Identifier> &&id_, NodePtr<Expression> &&init_)
      : Node(Kind_VariableDeclarator),
        id(std::move(id_)),
        init(std::move(init_)) {}
};

struct VariableDeclaration : public Declaration {
  // TODO kind
  VariableDeclaration() : Declaration(Kind_VariableDeclaration) {}
};

struct BlockStatement : public Statement {
  NodeVector<Statement> body;

 public:
  BlockStatement() : Statement(Kind_BlockStatement) {}

  static bool classof(const Node *node) {
    return node->Kind() == Kind_BlockStatement;
  }

  // void Accept(Visitor *visitor) override;
 protected:
  using Statement::Statement;
};

struct ReturnStatement : public Statement {
  NodePtr<Expression> argument;

  static bool classof(const Node *node) {
    return node->Kind() == Kind_ReturnStatement;
  }

  ReturnStatement(NodePtr<Expression> &&arg)
      : Statement(Kind_ReturnStatement), argument(std::move(arg)) {}
};

struct ExpressionStatement : public Statement {
  NodePtr<Expression> expression;

  ExpressionStatement(NodePtr<Expression> &&expr)
      : Statement(Kind_ExpressionStatement), expression(std::move(expr)) {}
};

struct Identifier : public Expression {
  lingo::Symbol const *symbol;

  static bool classof(const Node *node) {
    return node->Kind() == Kind_Identifier;
  }
  Identifier(const lingo::Symbol *symbol_)
      : Expression(Kind_Identifier), symbol(symbol_) {}
  // protected:
  // using Expression::Expression;
};

struct Literal : public Expression {
  lingo::Symbol const *symbol;

  static bool classof(const Node *node) { return node->Kind() == Kind_Literal; }
  Literal(const lingo::Symbol *symbol_)
      : Expression(Kind_Literal), symbol(symbol_) {}
};

// Does not inherit from Node to avoid the diamond problem
struct Function {
  NodePtr<Identifier> id;
  NodeVector<Identifier> params;
  NodePtr<BlockStatement> body;
};

struct FunctionDeclaration : public Statement, public Function {
  // NodePtr<Identifier> id;
  // NodeVector<Expression> params;
  // NodePtr<BlockStatement> body;
  static bool classof(const Node *node) {
    return node->Kind() == Kind_FunctionDeclaration;
  }
  FunctionDeclaration() : Statement(Kind_FunctionDeclaration) {}
};

struct IfStatement : public Statement {
  NodePtr<Expression> test;
  NodePtr<Statement> consequent;
  NodePtr<Statement> alternate;

  IfStatement(NodePtr<Expression> &&test_, NodePtr<Statement> consequent_)
      : Statement(Kind_IfStatement),
        test(std::move(test_)),
        consequent(std::move(consequent_)),
        alternate(nullptr) {}
  IfStatement(NodePtr<Expression> &&test_, NodePtr<Statement> consequent_,
              NodePtr<Statement> alternate_)
      : Statement(Kind_IfStatement),
        test(std::move(test_)),
        consequent(std::move(consequent_)),
        alternate(std::move(alternate_)) {}
};

struct ForStatement : public Statement {
  NodePtr<Node> init;
  NodePtr<Expression> test;
  NodePtr<Expression> update;
  NodePtr<Statement> body;

  ForStatement(NodePtr<Expression> &&init_, NodePtr<Expression> &&test_,
               NodePtr<Expression> &&update_, NodePtr<Statement> &&body_)
      : Statement(Kind_ForStatement),
        init(std::move(init_)),
        test(std::move(test_)),
        update(std::move(update_)),
        body(std::move(body_)) {}
  ForStatement(NodePtr<VariableDeclaration> &&init_,
               NodePtr<Expression> &&test_, NodePtr<Expression> &&update_,
               NodePtr<Statement> &&body_)
      : Statement(Kind_ForStatement),
        init(std::move(init_)),
        test(std::move(test_)),
        update(std::move(update_)),
        body(std::move(body_)) {}
};
}  // namespace ast
}  // namespace c8

#endif
