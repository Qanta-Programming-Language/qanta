use std::rc::Rc;

use super::{expr::LiteralValue, stmt::Stmt, tokenizer::Token};

pub type QantaResult<'a> = Result<(), String>;
pub type QantaAnalyzeResult<'a> = Result<&'a Vec<Token>, String>;
pub type QantaParserResult<'a> = Result<&'a Vec<Stmt>, String>;
pub type QantaInternalParserResult = Result<Stmt, String>;

pub type QantaFunction = Rc<dyn Fn(&[LiteralValue]) -> LiteralValue>;
