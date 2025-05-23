pub mod constants;
pub mod environment;
pub mod expr;
pub mod interpreter;
pub mod libraries;
pub mod panic;
pub mod parser;
pub mod resolver;
pub mod stmt;
pub mod tokenizer;
pub mod types;
pub mod utils;

use self::{
    constants::{QANTA_FILE_SUFFIX, QANTA_OK},
    interpreter::*,
    panic::PanicHandler,
    parser::QantaParser,
    resolver::Resolver,
    stmt::Stmt,
    tokenizer::{QantaTokenizer, Token},
    types::QantaResult,
    utils::formatter,
};

use std::{collections::HashMap, fs::read_to_string, path::Path, process::exit};

use clap::{
    builder::{styling::AnsiColor, Styles},
    crate_version, Arg, ArgMatches,
    ColorChoice::Always,
    Command,
};

use colored::*;
use webbrowser::open;

pub struct Qanta;

impl Qanta {
    pub fn run(&self) {
        let matches: ArgMatches = Command::new("")
            .bin_name("qanta || qt.exe")
            .about(self.about())
            .arg_required_else_help(true)
            .color(Always)
            .styles(self.styles())
            .subcommand(
                Command::new("run")
                    .arg(
                        Arg::new("path")
                            .help_heading("The direction of the file to execute.")
                            .required(true)
                            .require_equals(false),
                    )
                    .about("Run a Qanta file."),
            )
            .subcommand(Command::new("doc").about("Search documentation for commands or errors."))
            .subcommand(Command::new("creator").about("View the talented developer."))
            .get_matches();

        self.analyze(&matches);
    }

    fn analyze(&self, matches: &ArgMatches) {
        match matches.subcommand() {
            Some(("run", matches)) => {
                if let Some(file_path) = matches.get_one::<String>("path") {
                    self.analyze_file(file_path);
                };
            }

            Some(("doc" | "docs", _)) => {
                println!(
                    "{}",
                    formatter(true, true, &["Comming Soon...".bold().bright_yellow()])
                );
            }

            Some(("creator", _)) => open("https://github.com/Choqlito-dev").unwrap_or(()),

            _ => PanicHandler::new(
                None,
                None,
                None,
                "Unknown command. View 'zyqt || zyqt.exe --help",
            )
            .panic(),
        };
    }

    fn analyze_file(&self, path: &str) {
        if !path.ends_with(QANTA_FILE_SUFFIX) {
            PanicHandler::new(
                None,
                None,
                None,
                "Uknown file extension. View 'zyqt || zyqt.exe --help'",
            )
            .panic()
        }

        if !Path::new(path).exists() {
            PanicHandler::new(
                None,
                None,
                None,
                "File not found. View 'zyqt || zyqt.exe --help'",
            )
            .panic()
        }

        if let Ok(cont) = read_to_string(path) {
            match self.run_file(&cont) {
                Ok(()) => exit(QANTA_OK),
                Err(any) => {
                    PanicHandler::new(None, None, None, any.as_str()).panic();
                }
            }
        }

        PanicHandler::new(
            None,
            None,
            None,
            "Uknown read error. View 'zyqt || zyqt.exe --help'",
        )
        .panic()
    }

    fn run_file(&self, content: &str) -> QantaResult {
        let mut interpreter: QantaInterpreter = QantaInterpreter::new();

        let mut tokenizer: QantaTokenizer = QantaTokenizer::new(content);
        let tokens: &Vec<Token> = tokenizer.analyze()?;

        let mut parser: QantaParser = QantaParser::new(tokens);
        let stmts: &Vec<Stmt> = parser.parse()?;

        let resolver: Resolver = Resolver::new();
        let locals: HashMap<usize, usize> = resolver.resolve(stmts.iter().as_slice())?;

        interpreter.resolve(locals);

        interpreter.interpret(stmts.iter().collect())?;

        Ok(())
    }

    fn styles(&self) -> Styles {
        Styles::styled()
            .header(AnsiColor::BrightBlack.on_default())
            .usage(AnsiColor::BrightBlack.on_default())
            .literal(AnsiColor::White.on_default())
            .placeholder(AnsiColor::BrightWhite.on_default())
            .error(AnsiColor::BrightRed.on_default())
            .invalid(AnsiColor::BrightRed.on_default())
    }

    fn about(&self) -> String {
        formatter(
            false,
            false,
            &[
                "Qanta Programming Language".bold().bright_black(),
                " (".bold(),
                "Dev ".bold().red(),
                crate_version!().bold(),
                ")\n\n".bold(),
                "Qanta Programming Language is a fast scripting interpreted language builded with the power of Rust 🦀."
                    .bold()
                    .bright_white(),
            ],
        )
    }
}
