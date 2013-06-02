# Copyright (c) 2011 Esteban Tovagliari

import sys
from PySide.QtCore import *
from PySide.QtGui import *

class highlighting_rule_t( object):
	def __init__(self, pattern, format):
		self.pattern = pattern
		self.format = format

class se_expr_highlighter_t( QSyntaxHighlighter):	
	def __init__( self, text_edit, expression, parent):
		super( se_expr_highlighter_t, self).__init__( parent)
		self.text_edit_ = text_edit
		self.expression_ = expression
		self.highlighting_rules_ = []
		
		function_format = QTextCharFormat()
		function_format.setForeground( Qt.red)
		function_format.setFontWeight( QFont.Bold)
		function_names = ['acos', 'asin', 'atan', 'atan2', 'cos', 'sin', 'tan', 'cbrt', 
		'sqrt', 'ceil', 'floor', 'round', 'trunc', 'exp', 'log', 'log10', 'pow', 
		'cycle', 'pick', 'choose', 'wchoose', 'abs', 'max', 'min', 'fmod', 
		'bias', 'boxstep', 'ccellnoise', 'cellnoise', 'cfbm', 'cfbm4', 'clamp', 
		'cnoise', 'cnoise4', 'compress', 'contrast', 'cperlin', 'cturbulence', 
		'cvoronoi', 'expand', 'fbm', 'fbm', 'fbm4', 'fit', 'gamma', 'gaussstep'
		, 'hash', 'hsltorgb', 'hsi', 'invert', 'linearstep', 'midhsi', 'mix', 
		'noise', 'perlin', 'pnoise', 'pvoronoi', 'rand', 'remap', 'rgbtohsl', 
		'smoothstep', 'snoise', 'snoise4', 'sperlin', 'turbulence', 
		'vfbm', 'vfbm4', 'vnoise', 'vnoise4', 'voronoi', 'vperlin', 'vturbulence',
		"acosd", "asind", "atand", "atan2d", "cosd", "sind", "tand", "acosh", "asinh",
		"atanh", "cosh", "sinh", "tanh", "deg", "rad", "hypot", "angle", "cross", "dist",
		"dot", "length", "norm", "ortho", "up", "rotate", "curve", "spline", "printf"]

		for i in function_names:
			self.highlighting_rules_.append( highlighting_rule_t( "\\b" + i + "\\b", function_format))

		const_format = QTextCharFormat()
		const_format.setForeground( Qt.blue)
		const_format.setFontWeight( QFont.Bold)
		const_keywords = [ "PI", "E"]

		for i in const_keywords:
			self.highlighting_rules_.append( highlighting_rule_t( "\\b" + i + "\\b", const_format))

		self.highlighting_rules_.append( highlighting_rule_t( "[-]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?", const_format))

		comment_format = QTextCharFormat()
		comment_format.setForeground( Qt.green);
		self.highlighting_rules_.append( highlighting_rule_t( QRegExp( "#[^\n]*"), comment_format))
		
		string_format = QTextCharFormat()
		string_format.setForeground( Qt.magenta)
		string_format.setFontItalic( True)
		self.highlighting_rules_.append( highlighting_rule_t( QRegExp( '\"[^\"]*\"'), string_format))		
		
	def highlightBlock( self, text):
		self.text_edit_.blockSignals( True)
		for r in self.highlighting_rules_:
			expression = QRegExp( r.pattern)
			index = expression.indexIn( text)
			
			while index >= 0:
				length = expression.matchedLength()
				self.setFormat( index, length, r.format)
				index = expression.indexIn( text, index + length)

		# highlight vars
		var_format = QTextCharFormat()
		var_format.setForeground( Qt.yellow);
		var_format.setFontWeight( QFont.Bold)
		var_expr = QRegExp( "\$[A-Za-z][A-Za-z0-9_]*\\b");
	
		index = var_expr.indexIn( text)

		while index >= 0:
			length = var_expr.matchedLength()
			self.setFormat( index, length, var_format)
			index = var_expr.indexIn( text, index + length)
		
		self.setCurrentBlockState( 0)
		self.text_edit_.blockSignals( False)

class expression_edit_widget_t( QPlainTextEdit):
	def __init__( self, expression, parent = None):
		super( expression_edit_widget_t, self).__init__( parent)
		self.expression_ = expression
		self.setPlainText( expression.getExpr())
		self.highlighter_ = se_expr_highlighter_t( self, expression, self.document())
		self.textChanged.connect( self.update_expression)
		self.errors_view_ = None

	def set_errors_view( self, view):
		self.errors_view_ = view;

	def toAsciiPlainText( self):
		return self.toPlainText().encode( 'ascii', 'replace')

	def update_expression( self):
		self.expression_.setExpr( self.toAsciiPlainText())
		
		self.errors_view_.clear()

		if not self.expression_.isValid():
			for e in self.expression_.getErrors():
				self.errors_view_.appendPlainText( e.error)

class expression_editor_t( QDialog):
	def __init__( self, expression, callbacks, parent = None):
		super( expression_editor_t, self).__init__( parent)
		
		self.callbacks_ = callbacks
		
		self.setWindowTitle( "Expression editor")

		self.resize( 523, 608)

		main_layout = QVBoxLayout( self)

		toolbar = QWidget( self)
		toolbar.setMinimumSize( QSize( 0, 30))
		toolbar.setMaximumSize( QSize( 16777215, 30))

		hlayout = QHBoxLayout(toolbar)
		hlayout.setContentsMargins( 0, 0, 0, 0)

		button = QToolButton( toolbar)
		button.setText( "Load...")
		button.clicked.connect( self.load_expression)
		hlayout.addWidget( button)

		button = QToolButton( toolbar)
		button.setText( "Save...")
		button.clicked.connect( self.save_expression)
		hlayout.addWidget( button)

		hspacer = QSpacerItem( 40, 20, QSizePolicy.Expanding, QSizePolicy.Minimum)
		hlayout.addItem( hspacer)

		main_layout.addWidget(toolbar)

		font = QFont()
		font.setFamily( "Courier")
		font.setFixedPitch( True)
		font.setPointSize( 12)
		
		fm = QFontMetrics( font)
		self.font_height_ = fm.height()

		self.splitter_ = QSplitter( self)
		self.splitter_.setOrientation( Qt.Vertical)

		edit_widget = QWidget( self.splitter_)
		vlayout = QVBoxLayout( edit_widget)
		vlayout.setContentsMargins( 0, 0, 0, 0)
		self.expr_edit_ = expression_edit_widget_t( expression, edit_widget)
		self.expr_edit_.setFont( font)
		self.expr_edit_.setTabStopWidth( 40)
		self.expr_edit_.cursorPositionChanged.connect( self.update_info)
		vlayout.addWidget( self.expr_edit_)

		self.expr_info_ = QLabel( edit_widget)
		sizep = QSizePolicy( QSizePolicy.Preferred, QSizePolicy.Preferred)
		sizep.setHorizontalStretch( 0)
		sizep.setVerticalStretch( 0)
		sizep.setHeightForWidth( self.expr_info_.sizePolicy().hasHeightForWidth())
		self.expr_info_.setSizePolicy( sizep)
		self.expr_info_.setFrameShape( QFrame.Panel)
		self.expr_info_.setFrameShadow( QFrame.Sunken)
		self.expr_info_.setAlignment( Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
		self.expr_info_.setText( "Line 1")

		vlayout.addWidget( self.expr_info_)

		self.splitter_.addWidget( edit_widget)
		self.splitter_.setStretchFactor( 0, 4);

		self.errors_view_ = QPlainTextEdit( self.splitter_)
		self.errors_view_.setUndoRedoEnabled( False)
		self.errors_view_.setReadOnly( True)
		self.errors_view_.setFont( font)
		self.errors_view_.setPlainText( "No errors")
		self.splitter_.addWidget( self.errors_view_)
		self.expr_edit_.set_errors_view( self.errors_view_)
		main_layout.addWidget( self.splitter_)

		self.button_box_ = QDialogButtonBox( self)
		self.button_box_.setOrientation( Qt.Horizontal)
		self.button_box_.setStandardButtons( QDialogButtonBox.Apply | QDialogButtonBox.Cancel | QDialogButtonBox.Ok)
		self.button_box_.setCenterButtons( False)
		self.button_box_.clicked.connect( self.button_clicked)
		main_layout.addWidget( self.button_box_)

	def set_expression( self, string):
		self.expr_edit_.setPlainText( string)

	def button_clicked( self, but):
		role = self.button_box_.buttonRole( but)

		if role == QDialogButtonBox.AcceptRole:
			self.callbacks_.accept( self.expr_edit_.toAsciiPlainText())
			self.done( 1)

		if role == QDialogButtonBox.RejectRole:
			self.callbacks_.cancel()
			self.done( 0)

		if role == QDialogButtonBox.ApplyRole:
			self.callbacks_.apply( self.expr_edit_.toAsciiPlainText())

	def closeEvent( self, event):
		self.callbacks_.cancel()
		self.done( 0)

	def resizeEvent( self, event):
		super( expression_editor_t, self).resizeEvent( event)
		self.update_info()

	def update_info( self):
		#we use a fixed width font, so this trick works.
		svalue = self.expr_edit_.verticalScrollBar().value() * self.font_height_
		yvalue = self.expr_edit_.cursorRect().top() + svalue
		line_num = int( yvalue / self.font_height_) + 1
		self.expr_info_.setText( "Line %d" % line_num)

	def load_expression(self):
		filename = QFileDialog.getOpenFileName( self, "Open expression", "", "SeExpr (*.se *.txt)")
		
		if filename[0]:
			file = QFile(filename[0])
			
			if not file.open( QFile.ReadOnly | QFile.Text):
				QMessageBox.warning( self, "Expression editor", "Cannot read file")
				return

			intext = QTextStream( file)
			self.expr_edit_.setPlainText( intext.readAll());
	
	def save_expression(self):
		filename = QFileDialog.getSaveFileName( self, "Save expression", "", "SeExpr (*.se *.txt)")

		if filename[0]:
			file = QFile(filename[0])
			
			if not file.open( QFile.WriteOnly | QFile.Text):
				QMessageBox.warning( self, "Expression editor", "Cannot open file")
				return

			outtext = QTextStream( file)
			outtext << self.expr_edit_.toAsciiPlainText()

def create_expression_editor( expr, callbacks, parent):
	form = expression_editor_t( expr, callbacks, parent)
	form.show()
	form.exec_()

if __name__ == '__main__':
	
	class mock_expression_t( object):
		def __init__( self):
			pass

		def getExpr( self):
			return ""

		def setExpr( self, text):
			pass

		def syntaxOK( self):
			return True

		def isValid( self):
			return True

		def usesVar( self, var):
			return True

		def getErrors( self):
			return []

	class mock_callback_suite_t( object):
		def __init__( self):
			pass
		
		def accept(self, expr):
			print "accepted"
		
		def apply( self, expr):
			print "apply"
		
		def cancel( self):
			print "cancel"
	
	app = QApplication( sys.argv)
	form = expression_editor_t( mock_expression_t(), mock_callback_suite_t())
	form.show()
	sys.exit( app.exec_())
