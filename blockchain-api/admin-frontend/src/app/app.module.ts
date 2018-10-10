/*
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { HttpModule } from '@angular/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { AppRoutingModule } from './app-routing.module';
import { DataService } from './data.service';
import { AppComponent } from './app.component';
import { HomeComponent } from './home/home.component';

import { EHRComponent } from './EHR/EHR.component';
import { PatientInfoComponent } from './PatientInfo/PatientInfo.component';
import { DoctorInfoComponent } from './DoctorInfo/DoctorInfo.component';

import { PatientComponent } from './Patient/Patient.component';
import { DoctorComponent } from './Doctor/Doctor.component';
import { InsuranceComponent } from './Insurance/Insurance.component';

import { ChangeInsuranceComponent } from './ChangeInsurance/ChangeInsurance.component';
import { RemoveDoctorViewAccessComponent } from './RemoveDoctorViewAccess/RemoveDoctorViewAccess.component';
import { AddDoctorViewAccessComponent } from './AddDoctorViewAccess/AddDoctorViewAccess.component';
import { DoctorUpdatePatientDataComponent } from './DoctorUpdatePatientData/DoctorUpdatePatientData.component';

  @NgModule({
  declarations: [
    AppComponent,
    HomeComponent,
    EHRComponent,
    PatientInfoComponent,
    DoctorInfoComponent,
    PatientComponent,
    DoctorComponent,
    InsuranceComponent,
    ChangeInsuranceComponent,
    RemoveDoctorViewAccessComponent,
    AddDoctorViewAccessComponent,
    DoctorUpdatePatientDataComponent
  ],
  imports: [
    BrowserModule,
    FormsModule,
    ReactiveFormsModule,
    HttpModule,
    AppRoutingModule
  ],
  providers: [
    DataService
  ],
  bootstrap: [AppComponent]
})
export class AppModule { }
