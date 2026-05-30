namespace API_Request_Builder
{
    partial class Form1
    {
        /// <summary>
        /// Variable nécessaire au concepteur.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Nettoyage des ressources utilisées.
        /// </summary>
        /// <param name="disposing">true si les ressources managées doivent être supprimées ; sinon, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Code généré par le Concepteur Windows Form

        /// <summary>
        /// Méthode requise pour la prise en charge du concepteur - ne modifiez pas
        /// le contenu de cette méthode avec l'éditeur de code.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.textBoxKey = new System.Windows.Forms.TextBox();
            this.textBoxSelectedStop = new System.Windows.Forms.TextBox();
            this.textBoxSelectedRoute = new System.Windows.Forms.TextBox();
            this.labelAPIKey = new System.Windows.Forms.Label();
            this.comboBoxRequestType = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.labelJSON = new System.Windows.Forms.Label();
            this.comboBoxAvailableStops = new System.Windows.Forms.ComboBox();
            this.labelStops = new System.Windows.Forms.Label();
            this.textBoxDataQuery = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.buttonDataQuery = new System.Windows.Forms.Button();
            this.buttonDeparturesQuery = new System.Windows.Forms.Button();
            this.textBoxDeparturesQuery = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label4 = new System.Windows.Forms.Label();
            this.comboBoxAvailableRoutes = new System.Windows.Forms.ComboBox();
            this.rtfBoxAllDepartures = new System.Windows.Forms.RichTextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.labelTime = new System.Windows.Forms.Label();
            this.labelCurrentTime = new System.Windows.Forms.Label();
            this.labelNextDepartures = new System.Windows.Forms.Label();
            this.textBoxNextDeparture1 = new System.Windows.Forms.TextBox();
            this.textBoxNextDeparture2 = new System.Windows.Forms.TextBox();
            this.textBoxNextDeparture3 = new System.Windows.Forms.TextBox();
            this.timerTimeUpdate = new System.Windows.Forms.Timer(this.components);
            this.buttonResetSelection = new System.Windows.Forms.Button();
            this.labelStatus = new System.Windows.Forms.Label();
            this.buttonConfirmSelection = new System.Windows.Forms.Button();
            this.labelStatusDepartures = new System.Windows.Forms.Label();
            this.comboBoxAvailableDirections = new System.Windows.Forms.ComboBox();
            this.label_directions = new System.Windows.Forms.Label();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.button_Connect = new System.Windows.Forms.Button();
            this.button_Epoch = new System.Windows.Forms.Button();
            this.comboBoxCOM = new System.Windows.Forms.ComboBox();
            this.rtf_UART = new System.Windows.Forms.RichTextBox();
            this.button_StopGood = new System.Windows.Forms.Button();
            this.button_LineGood = new System.Windows.Forms.Button();
            this.button_NbrLines = new System.Windows.Forms.Button();
            this.button_LineID = new System.Windows.Forms.Button();
            this.button_Directions = new System.Windows.Forms.Button();
            this.button_NbrStops = new System.Windows.Forms.Button();
            this.button_StopID = new System.Windows.Forms.Button();
            this.button_StopBad = new System.Windows.Forms.Button();
            this.button_LineBad = new System.Windows.Forms.Button();
            this.button_TimeRemaining = new System.Windows.Forms.Button();
            this.button_DoneSending = new System.Windows.Forms.Button();
            this.rtfBox_json = new System.Windows.Forms.RichTextBox();
            this.button_saveToCache = new System.Windows.Forms.Button();
            this.button_reloadFromCache = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // textBoxKey
            // 
            this.textBoxKey.Location = new System.Drawing.Point(135, 14);
            this.textBoxKey.Name = "textBoxKey";
            this.textBoxKey.Size = new System.Drawing.Size(562, 20);
            this.textBoxKey.TabIndex = 0;
            // 
            // textBoxSelectedStop
            // 
            this.textBoxSelectedStop.Location = new System.Drawing.Point(410, 325);
            this.textBoxSelectedStop.Name = "textBoxSelectedStop";
            this.textBoxSelectedStop.Size = new System.Drawing.Size(155, 20);
            this.textBoxSelectedStop.TabIndex = 3;
            // 
            // textBoxSelectedRoute
            // 
            this.textBoxSelectedRoute.Location = new System.Drawing.Point(244, 324);
            this.textBoxSelectedRoute.Name = "textBoxSelectedRoute";
            this.textBoxSelectedRoute.Size = new System.Drawing.Size(82, 20);
            this.textBoxSelectedRoute.TabIndex = 4;
            // 
            // labelAPIKey
            // 
            this.labelAPIKey.AutoSize = true;
            this.labelAPIKey.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelAPIKey.Location = new System.Drawing.Point(12, 13);
            this.labelAPIKey.Name = "labelAPIKey";
            this.labelAPIKey.Size = new System.Drawing.Size(81, 19);
            this.labelAPIKey.TabIndex = 5;
            this.labelAPIKey.Text = "API Key:";
            // 
            // comboBoxRequestType
            // 
            this.comboBoxRequestType.FormattingEnabled = true;
            this.comboBoxRequestType.Location = new System.Drawing.Point(135, 40);
            this.comboBoxRequestType.Name = "comboBoxRequestType";
            this.comboBoxRequestType.Size = new System.Drawing.Size(109, 21);
            this.comboBoxRequestType.TabIndex = 9;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label1.Location = new System.Drawing.Point(12, 42);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(99, 19);
            this.label1.TabIndex = 10;
            this.label1.Text = "cURL / iwr";
            // 
            // labelJSON
            // 
            this.labelJSON.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.labelJSON.AutoSize = true;
            this.labelJSON.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelJSON.Location = new System.Drawing.Point(589, 485);
            this.labelJSON.Name = "labelJSON";
            this.labelJSON.Size = new System.Drawing.Size(108, 19);
            this.labelJSON.TabIndex = 11;
            this.labelJSON.Text = "json output";
            // 
            // comboBoxAvailableStops
            // 
            this.comboBoxAvailableStops.FormattingEnabled = true;
            this.comboBoxAvailableStops.Location = new System.Drawing.Point(524, 171);
            this.comboBoxAvailableStops.Name = "comboBoxAvailableStops";
            this.comboBoxAvailableStops.Size = new System.Drawing.Size(155, 21);
            this.comboBoxAvailableStops.TabIndex = 12;
            this.comboBoxAvailableStops.SelectedIndexChanged += new System.EventHandler(this.comboBoxAvailableStops_SelectedIndexChanged);
            // 
            // labelStops
            // 
            this.labelStops.AutoSize = true;
            this.labelStops.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelStops.Location = new System.Drawing.Point(338, 170);
            this.labelStops.Name = "labelStops";
            this.labelStops.Size = new System.Drawing.Size(180, 19);
            this.labelStops.TabIndex = 13;
            this.labelStops.Text = "<=> Available Stops";
            // 
            // textBoxDataQuery
            // 
            this.textBoxDataQuery.Location = new System.Drawing.Point(171, 76);
            this.textBoxDataQuery.Name = "textBoxDataQuery";
            this.textBoxDataQuery.Size = new System.Drawing.Size(347, 20);
            this.textBoxDataQuery.TabIndex = 15;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.Location = new System.Drawing.Point(12, 76);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(108, 19);
            this.label2.TabIndex = 14;
            this.label2.Text = "Data Query:";
            // 
            // buttonDataQuery
            // 
            this.buttonDataQuery.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.buttonDataQuery.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonDataQuery.Location = new System.Drawing.Point(542, 68);
            this.buttonDataQuery.Name = "buttonDataQuery";
            this.buttonDataQuery.Size = new System.Drawing.Size(155, 31);
            this.buttonDataQuery.TabIndex = 16;
            this.buttonDataQuery.Text = "Build Data";
            this.buttonDataQuery.UseVisualStyleBackColor = false;
            this.buttonDataQuery.Click += new System.EventHandler(this.buttonDataQuery_Click);
            // 
            // buttonDeparturesQuery
            // 
            this.buttonDeparturesQuery.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(255)))));
            this.buttonDeparturesQuery.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonDeparturesQuery.Location = new System.Drawing.Point(542, 274);
            this.buttonDeparturesQuery.Name = "buttonDeparturesQuery";
            this.buttonDeparturesQuery.Size = new System.Drawing.Size(155, 31);
            this.buttonDeparturesQuery.TabIndex = 21;
            this.buttonDeparturesQuery.Text = "Find Departures";
            this.buttonDeparturesQuery.UseVisualStyleBackColor = false;
            this.buttonDeparturesQuery.Click += new System.EventHandler(this.buttonDeparturesQuery_Click);
            // 
            // textBoxDeparturesQuery
            // 
            this.textBoxDeparturesQuery.Location = new System.Drawing.Point(171, 281);
            this.textBoxDeparturesQuery.Name = "textBoxDeparturesQuery";
            this.textBoxDeparturesQuery.Size = new System.Drawing.Size(347, 20);
            this.textBoxDeparturesQuery.TabIndex = 20;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label3.Location = new System.Drawing.Point(12, 282);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(135, 19);
            this.label3.TabIndex = 19;
            this.label3.Text = "Depart. Query:";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label4.Location = new System.Drawing.Point(12, 168);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(153, 19);
            this.label4.TabIndex = 18;
            this.label4.Text = "Available Routes";
            // 
            // comboBoxAvailableRoutes
            // 
            this.comboBoxAvailableRoutes.FormattingEnabled = true;
            this.comboBoxAvailableRoutes.Location = new System.Drawing.Point(171, 168);
            this.comboBoxAvailableRoutes.Name = "comboBoxAvailableRoutes";
            this.comboBoxAvailableRoutes.Size = new System.Drawing.Size(155, 21);
            this.comboBoxAvailableRoutes.TabIndex = 17;
            this.comboBoxAvailableRoutes.SelectedIndexChanged += new System.EventHandler(this.comboBoxAvailableRoutes_SelectedIndexChanged);
            // 
            // rtfBoxAllDepartures
            // 
            this.rtfBoxAllDepartures.Location = new System.Drawing.Point(12, 350);
            this.rtfBoxAllDepartures.Name = "rtfBoxAllDepartures";
            this.rtfBoxAllDepartures.Size = new System.Drawing.Size(685, 74);
            this.rtfBoxAllDepartures.TabIndex = 22;
            this.rtfBoxAllDepartures.Text = "";
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label5.Location = new System.Drawing.Point(13, 323);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(225, 19);
            this.label5.TabIndex = 23;
            this.label5.Text = "All departures for route";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label6.Location = new System.Drawing.Point(332, 324);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(72, 19);
            this.label6.TabIndex = 24;
            this.label6.Text = "on stop";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label7.Location = new System.Drawing.Point(571, 324);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(18, 19);
            this.label7.TabIndex = 25;
            this.label7.Text = ":";
            // 
            // labelTime
            // 
            this.labelTime.AutoSize = true;
            this.labelTime.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelTime.Location = new System.Drawing.Point(12, 441);
            this.labelTime.Name = "labelTime";
            this.labelTime.Size = new System.Drawing.Size(144, 19);
            this.labelTime.TabIndex = 26;
            this.labelTime.Text = "Current time  :";
            // 
            // labelCurrentTime
            // 
            this.labelCurrentTime.AutoSize = true;
            this.labelCurrentTime.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelCurrentTime.Location = new System.Drawing.Point(167, 441);
            this.labelCurrentTime.Name = "labelCurrentTime";
            this.labelCurrentTime.Size = new System.Drawing.Size(45, 19);
            this.labelCurrentTime.TabIndex = 27;
            this.labelCurrentTime.Text = "TIME";
            // 
            // labelNextDepartures
            // 
            this.labelNextDepartures.AutoSize = true;
            this.labelNextDepartures.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelNextDepartures.Location = new System.Drawing.Point(13, 476);
            this.labelNextDepartures.Name = "labelNextDepartures";
            this.labelNextDepartures.Size = new System.Drawing.Size(261, 19);
            this.labelNextDepartures.TabIndex = 28;
            this.labelNextDepartures.Text = "Next three route departures:";
            // 
            // textBoxNextDeparture1
            // 
            this.textBoxNextDeparture1.Location = new System.Drawing.Point(20, 509);
            this.textBoxNextDeparture1.Name = "textBoxNextDeparture1";
            this.textBoxNextDeparture1.Size = new System.Drawing.Size(109, 20);
            this.textBoxNextDeparture1.TabIndex = 29;
            // 
            // textBoxNextDeparture2
            // 
            this.textBoxNextDeparture2.Location = new System.Drawing.Point(165, 509);
            this.textBoxNextDeparture2.Name = "textBoxNextDeparture2";
            this.textBoxNextDeparture2.Size = new System.Drawing.Size(109, 20);
            this.textBoxNextDeparture2.TabIndex = 30;
            // 
            // textBoxNextDeparture3
            // 
            this.textBoxNextDeparture3.Location = new System.Drawing.Point(312, 509);
            this.textBoxNextDeparture3.Name = "textBoxNextDeparture3";
            this.textBoxNextDeparture3.Size = new System.Drawing.Size(109, 20);
            this.textBoxNextDeparture3.TabIndex = 31;
            // 
            // timerTimeUpdate
            // 
            this.timerTimeUpdate.Enabled = true;
            this.timerTimeUpdate.Interval = 250;
            this.timerTimeUpdate.Tick += new System.EventHandler(this.timerTimeUpdate_Tick);
            // 
            // buttonResetSelection
            // 
            this.buttonResetSelection.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(128)))));
            this.buttonResetSelection.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonResetSelection.Location = new System.Drawing.Point(374, 229);
            this.buttonResetSelection.Name = "buttonResetSelection";
            this.buttonResetSelection.Size = new System.Drawing.Size(155, 31);
            this.buttonResetSelection.TabIndex = 32;
            this.buttonResetSelection.Text = "Reset Selection";
            this.buttonResetSelection.UseVisualStyleBackColor = false;
            this.buttonResetSelection.Click += new System.EventHandler(this.buttonResetSelection_Click);
            // 
            // labelStatus
            // 
            this.labelStatus.AutoSize = true;
            this.labelStatus.Font = new System.Drawing.Font("Consolas", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelStatus.Location = new System.Drawing.Point(554, 102);
            this.labelStatus.Name = "labelStatus";
            this.labelStatus.Size = new System.Drawing.Size(126, 15);
            this.labelStatus.TabIndex = 33;
            this.labelStatus.Text = "Request Completed";
            // 
            // buttonConfirmSelection
            // 
            this.buttonConfirmSelection.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(255)))));
            this.buttonConfirmSelection.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.buttonConfirmSelection.Location = new System.Drawing.Point(89, 229);
            this.buttonConfirmSelection.Name = "buttonConfirmSelection";
            this.buttonConfirmSelection.Size = new System.Drawing.Size(185, 31);
            this.buttonConfirmSelection.TabIndex = 34;
            this.buttonConfirmSelection.Text = "Confirm Selection";
            this.buttonConfirmSelection.UseVisualStyleBackColor = false;
            this.buttonConfirmSelection.Click += new System.EventHandler(this.buttonConfirmSelection_Click);
            // 
            // labelStatusDepartures
            // 
            this.labelStatusDepartures.AutoSize = true;
            this.labelStatusDepartures.Font = new System.Drawing.Font("Consolas", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.labelStatusDepartures.Location = new System.Drawing.Point(554, 307);
            this.labelStatusDepartures.Name = "labelStatusDepartures";
            this.labelStatusDepartures.Size = new System.Drawing.Size(126, 15);
            this.labelStatusDepartures.TabIndex = 35;
            this.labelStatusDepartures.Text = "Request Completed";
            // 
            // comboBoxAvailableDirections
            // 
            this.comboBoxAvailableDirections.FormattingEnabled = true;
            this.comboBoxAvailableDirections.Location = new System.Drawing.Point(223, 203);
            this.comboBoxAvailableDirections.Name = "comboBoxAvailableDirections";
            this.comboBoxAvailableDirections.Size = new System.Drawing.Size(249, 21);
            this.comboBoxAvailableDirections.TabIndex = 36;
            this.comboBoxAvailableDirections.SelectedIndexChanged += new System.EventHandler(this.comboBoxAvailableDirections_SelectedIndexChanged);
            // 
            // label_directions
            // 
            this.label_directions.AutoSize = true;
            this.label_directions.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label_directions.Location = new System.Drawing.Point(15, 202);
            this.label_directions.Name = "label_directions";
            this.label_directions.Size = new System.Drawing.Size(189, 19);
            this.label_directions.TabIndex = 37;
            this.label_directions.Text = "Available Directions";
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 115200;
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // button_Connect
            // 
            this.button_Connect.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.button_Connect.Location = new System.Drawing.Point(788, 16);
            this.button_Connect.Name = "button_Connect";
            this.button_Connect.Size = new System.Drawing.Size(103, 47);
            this.button_Connect.TabIndex = 38;
            this.button_Connect.Text = "Connect";
            this.button_Connect.UseVisualStyleBackColor = true;
            this.button_Connect.Click += new System.EventHandler(this.button_Connect_Click);
            // 
            // button_Epoch
            // 
            this.button_Epoch.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_Epoch.Location = new System.Drawing.Point(788, 91);
            this.button_Epoch.Name = "button_Epoch";
            this.button_Epoch.Size = new System.Drawing.Size(212, 38);
            this.button_Epoch.TabIndex = 39;
            this.button_Epoch.Text = "Epoch";
            this.button_Epoch.UseVisualStyleBackColor = true;
            this.button_Epoch.Click += new System.EventHandler(this.button_Epoch_Click);
            // 
            // comboBoxCOM
            // 
            this.comboBoxCOM.FormattingEnabled = true;
            this.comboBoxCOM.Location = new System.Drawing.Point(897, 31);
            this.comboBoxCOM.Name = "comboBoxCOM";
            this.comboBoxCOM.Size = new System.Drawing.Size(125, 21);
            this.comboBoxCOM.TabIndex = 40;
            // 
            // rtf_UART
            // 
            this.rtf_UART.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.rtf_UART.Location = new System.Drawing.Point(1053, 31);
            this.rtf_UART.Name = "rtf_UART";
            this.rtf_UART.Size = new System.Drawing.Size(328, 500);
            this.rtf_UART.TabIndex = 41;
            this.rtf_UART.Text = "";
            // 
            // button_StopGood
            // 
            this.button_StopGood.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.button_StopGood.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_StopGood.Location = new System.Drawing.Point(788, 137);
            this.button_StopGood.Name = "button_StopGood";
            this.button_StopGood.Size = new System.Drawing.Size(103, 38);
            this.button_StopGood.TabIndex = 42;
            this.button_StopGood.Text = "Stop Good";
            this.button_StopGood.UseVisualStyleBackColor = false;
            this.button_StopGood.Click += new System.EventHandler(this.button_StopGood_Click);
            // 
            // button_LineGood
            // 
            this.button_LineGood.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.button_LineGood.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_LineGood.Location = new System.Drawing.Point(788, 181);
            this.button_LineGood.Name = "button_LineGood";
            this.button_LineGood.Size = new System.Drawing.Size(103, 38);
            this.button_LineGood.TabIndex = 43;
            this.button_LineGood.Text = "Line Good";
            this.button_LineGood.UseVisualStyleBackColor = false;
            this.button_LineGood.Click += new System.EventHandler(this.button_LineGood_Click);
            // 
            // button_NbrLines
            // 
            this.button_NbrLines.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(255)))));
            this.button_NbrLines.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_NbrLines.Location = new System.Drawing.Point(788, 225);
            this.button_NbrLines.Name = "button_NbrLines";
            this.button_NbrLines.Size = new System.Drawing.Size(212, 38);
            this.button_NbrLines.TabIndex = 44;
            this.button_NbrLines.Text = "# of lines on stop";
            this.button_NbrLines.UseVisualStyleBackColor = false;
            this.button_NbrLines.Click += new System.EventHandler(this.button_NbrLines_Click);
            // 
            // button_LineID
            // 
            this.button_LineID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(192)))), ((int)(((byte)(255)))));
            this.button_LineID.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_LineID.Location = new System.Drawing.Point(788, 272);
            this.button_LineID.Name = "button_LineID";
            this.button_LineID.Size = new System.Drawing.Size(212, 38);
            this.button_LineID.TabIndex = 45;
            this.button_LineID.Text = "Line identifier";
            this.button_LineID.UseVisualStyleBackColor = false;
            this.button_LineID.Click += new System.EventHandler(this.button_LineID_Click);
            // 
            // button_Directions
            // 
            this.button_Directions.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_Directions.Location = new System.Drawing.Point(788, 316);
            this.button_Directions.Name = "button_Directions";
            this.button_Directions.Size = new System.Drawing.Size(212, 38);
            this.button_Directions.TabIndex = 46;
            this.button_Directions.Text = "Directions on line";
            this.button_Directions.UseVisualStyleBackColor = true;
            this.button_Directions.Click += new System.EventHandler(this.button_Directions_Click);
            // 
            // button_NbrStops
            // 
            this.button_NbrStops.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(128)))));
            this.button_NbrStops.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_NbrStops.Location = new System.Drawing.Point(788, 360);
            this.button_NbrStops.Name = "button_NbrStops";
            this.button_NbrStops.Size = new System.Drawing.Size(212, 38);
            this.button_NbrStops.TabIndex = 47;
            this.button_NbrStops.Text = "# of stops on line";
            this.button_NbrStops.UseVisualStyleBackColor = false;
            this.button_NbrStops.Click += new System.EventHandler(this.button_NbrStops_Click);
            // 
            // button_StopID
            // 
            this.button_StopID.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(255)))), ((int)(((byte)(128)))));
            this.button_StopID.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_StopID.Location = new System.Drawing.Point(788, 404);
            this.button_StopID.Name = "button_StopID";
            this.button_StopID.Size = new System.Drawing.Size(212, 38);
            this.button_StopID.TabIndex = 48;
            this.button_StopID.Text = "Stop identifier";
            this.button_StopID.UseVisualStyleBackColor = false;
            this.button_StopID.Click += new System.EventHandler(this.button_StopID_Click);
            // 
            // button_StopBad
            // 
            this.button_StopBad.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.button_StopBad.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_StopBad.Location = new System.Drawing.Point(897, 136);
            this.button_StopBad.Name = "button_StopBad";
            this.button_StopBad.Size = new System.Drawing.Size(103, 38);
            this.button_StopBad.TabIndex = 49;
            this.button_StopBad.Text = "Stop Bad";
            this.button_StopBad.UseVisualStyleBackColor = false;
            this.button_StopBad.Click += new System.EventHandler(this.button_StopBad_Click);
            // 
            // button_LineBad
            // 
            this.button_LineBad.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(192)))), ((int)(((byte)(192)))));
            this.button_LineBad.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_LineBad.Location = new System.Drawing.Point(897, 181);
            this.button_LineBad.Name = "button_LineBad";
            this.button_LineBad.Size = new System.Drawing.Size(103, 38);
            this.button_LineBad.TabIndex = 50;
            this.button_LineBad.Text = "Line Bad";
            this.button_LineBad.UseVisualStyleBackColor = false;
            this.button_LineBad.Click += new System.EventHandler(this.button_LineBad_Click);
            // 
            // button_TimeRemaining
            // 
            this.button_TimeRemaining.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_TimeRemaining.Location = new System.Drawing.Point(788, 448);
            this.button_TimeRemaining.Name = "button_TimeRemaining";
            this.button_TimeRemaining.Size = new System.Drawing.Size(212, 38);
            this.button_TimeRemaining.TabIndex = 51;
            this.button_TimeRemaining.Text = "Time remaining";
            this.button_TimeRemaining.UseVisualStyleBackColor = true;
            this.button_TimeRemaining.Click += new System.EventHandler(this.button_TimeRemaining_Click);
            // 
            // button_DoneSending
            // 
            this.button_DoneSending.BackColor = System.Drawing.Color.Silver;
            this.button_DoneSending.Font = new System.Drawing.Font("Consolas", 12F, System.Drawing.FontStyle.Bold);
            this.button_DoneSending.Location = new System.Drawing.Point(788, 493);
            this.button_DoneSending.Name = "button_DoneSending";
            this.button_DoneSending.Size = new System.Drawing.Size(212, 38);
            this.button_DoneSending.TabIndex = 52;
            this.button_DoneSending.Text = "Done sending";
            this.button_DoneSending.UseVisualStyleBackColor = false;
            this.button_DoneSending.Click += new System.EventHandler(this.button_DoneSending_Click);
            // 
            // rtfBox_json
            // 
            this.rtfBox_json.Anchor = System.Windows.Forms.AnchorStyles.None;
            this.rtfBox_json.BackColor = System.Drawing.SystemColors.InactiveCaption;
            this.rtfBox_json.Location = new System.Drawing.Point(723, 16);
            this.rtfBox_json.Name = "rtfBox_json";
            this.rtfBox_json.Size = new System.Drawing.Size(39, 38);
            this.rtfBox_json.TabIndex = 6;
            this.rtfBox_json.Text = "";
            // 
            // button_saveToCache
            // 
            this.button_saveToCache.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(192)))), ((int)(((byte)(255)))), ((int)(((byte)(192)))));
            this.button_saveToCache.Location = new System.Drawing.Point(542, 120);
            this.button_saveToCache.Name = "button_saveToCache";
            this.button_saveToCache.Size = new System.Drawing.Size(155, 23);
            this.button_saveToCache.TabIndex = 53;
            this.button_saveToCache.Text = "Save data to cache";
            this.button_saveToCache.UseVisualStyleBackColor = false;
            this.button_saveToCache.Click += new System.EventHandler(this.button_saveToCache_Click);
            // 
            // button_reloadFromCache
            // 
            this.button_reloadFromCache.BackColor = System.Drawing.Color.FromArgb(((int)(((byte)(255)))), ((int)(((byte)(224)))), ((int)(((byte)(192)))));
            this.button_reloadFromCache.Location = new System.Drawing.Point(374, 120);
            this.button_reloadFromCache.Name = "button_reloadFromCache";
            this.button_reloadFromCache.Size = new System.Drawing.Size(155, 23);
            this.button_reloadFromCache.TabIndex = 54;
            this.button_reloadFromCache.Text = "Reload from cache";
            this.button_reloadFromCache.UseVisualStyleBackColor = false;
            this.button_reloadFromCache.Click += new System.EventHandler(this.button_reloadFromCache_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1397, 543);
            this.Controls.Add(this.button_reloadFromCache);
            this.Controls.Add(this.button_saveToCache);
            this.Controls.Add(this.button_DoneSending);
            this.Controls.Add(this.button_TimeRemaining);
            this.Controls.Add(this.button_LineBad);
            this.Controls.Add(this.button_StopBad);
            this.Controls.Add(this.button_StopID);
            this.Controls.Add(this.button_NbrStops);
            this.Controls.Add(this.button_Directions);
            this.Controls.Add(this.button_LineID);
            this.Controls.Add(this.button_NbrLines);
            this.Controls.Add(this.button_LineGood);
            this.Controls.Add(this.button_StopGood);
            this.Controls.Add(this.rtf_UART);
            this.Controls.Add(this.comboBoxCOM);
            this.Controls.Add(this.button_Epoch);
            this.Controls.Add(this.button_Connect);
            this.Controls.Add(this.label_directions);
            this.Controls.Add(this.comboBoxAvailableDirections);
            this.Controls.Add(this.labelStatusDepartures);
            this.Controls.Add(this.buttonConfirmSelection);
            this.Controls.Add(this.labelStatus);
            this.Controls.Add(this.buttonResetSelection);
            this.Controls.Add(this.textBoxNextDeparture3);
            this.Controls.Add(this.textBoxNextDeparture2);
            this.Controls.Add(this.textBoxNextDeparture1);
            this.Controls.Add(this.labelNextDepartures);
            this.Controls.Add(this.labelCurrentTime);
            this.Controls.Add(this.labelTime);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.rtfBoxAllDepartures);
            this.Controls.Add(this.buttonDeparturesQuery);
            this.Controls.Add(this.textBoxDeparturesQuery);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.comboBoxAvailableRoutes);
            this.Controls.Add(this.buttonDataQuery);
            this.Controls.Add(this.textBoxDataQuery);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.labelStops);
            this.Controls.Add(this.comboBoxAvailableStops);
            this.Controls.Add(this.labelJSON);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.comboBoxRequestType);
            this.Controls.Add(this.rtfBox_json);
            this.Controls.Add(this.labelAPIKey);
            this.Controls.Add(this.textBoxSelectedRoute);
            this.Controls.Add(this.textBoxSelectedStop);
            this.Controls.Add(this.textBoxKey);
            this.Name = "Form1";
            this.Text = "API Request Tester";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox textBoxKey;
        private System.Windows.Forms.TextBox textBoxSelectedStop;
        private System.Windows.Forms.TextBox textBoxSelectedRoute;
        private System.Windows.Forms.Label labelAPIKey;
        private System.Windows.Forms.ComboBox comboBoxRequestType;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label labelJSON;
        private System.Windows.Forms.ComboBox comboBoxAvailableStops;
        private System.Windows.Forms.Label labelStops;
        private System.Windows.Forms.TextBox textBoxDataQuery;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button buttonDataQuery;
        private System.Windows.Forms.Button buttonDeparturesQuery;
        private System.Windows.Forms.TextBox textBoxDeparturesQuery;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.ComboBox comboBoxAvailableRoutes;
        private System.Windows.Forms.RichTextBox rtfBoxAllDepartures;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.Label labelTime;
        private System.Windows.Forms.Label labelCurrentTime;
        private System.Windows.Forms.Label labelNextDepartures;
        private System.Windows.Forms.TextBox textBoxNextDeparture1;
        private System.Windows.Forms.TextBox textBoxNextDeparture2;
        private System.Windows.Forms.TextBox textBoxNextDeparture3;
        private System.Windows.Forms.Timer timerTimeUpdate;
        private System.Windows.Forms.Button buttonResetSelection;
        private System.Windows.Forms.Label labelStatus;
        private System.Windows.Forms.Button buttonConfirmSelection;
        private System.Windows.Forms.Label labelStatusDepartures;
        private System.Windows.Forms.ComboBox comboBoxAvailableDirections;
        private System.Windows.Forms.Label label_directions;
        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Button button_Connect;
        private System.Windows.Forms.Button button_Epoch;
        private System.Windows.Forms.ComboBox comboBoxCOM;
        private System.Windows.Forms.RichTextBox rtf_UART;
        private System.Windows.Forms.Button button_StopGood;
        private System.Windows.Forms.Button button_LineGood;
        private System.Windows.Forms.Button button_NbrLines;
        private System.Windows.Forms.Button button_LineID;
        private System.Windows.Forms.Button button_Directions;
        private System.Windows.Forms.Button button_NbrStops;
        private System.Windows.Forms.Button button_StopID;
        private System.Windows.Forms.Button button_StopBad;
        private System.Windows.Forms.Button button_LineBad;
        private System.Windows.Forms.Button button_TimeRemaining;
        private System.Windows.Forms.Button button_DoneSending;
        private System.Windows.Forms.RichTextBox rtfBox_json;
        private System.Windows.Forms.Button button_saveToCache;
        private System.Windows.Forms.Button button_reloadFromCache;
    }
}

