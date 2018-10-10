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

import { NgModule } from '@angular/core';
import { Routes, RouterModule } from '@angular/router';

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

const routes: Routes = [
  { path: '', component: HomeComponent },
  { path: 'EHR', component: EHRComponent },
  { path: 'PatientInfo', component: PatientInfoComponent },
  { path: 'DoctorInfo', component: DoctorInfoComponent },
  { path: 'Patient', component: PatientComponent },
  { path: 'Doctor', component: DoctorComponent },
  { path: 'Insurance', component: InsuranceComponent },
  { path: 'ChangeInsurance', component: ChangeInsuranceComponent },
  { path: 'RemoveDoctorViewAccess', component: RemoveDoctorViewAccessComponent },
  { path: 'AddDoctorViewAccess', component: AddDoctorViewAccessComponent },
  { path: 'DoctorUpdatePatientData', component: DoctorUpdatePatientDataComponent },
  { path: '**', redirectTo: '' }
];

@NgModule({
 imports: [RouterModule.forRoot(routes)],
 exports: [RouterModule],
 providers: []
})
export class AppRoutingModule { }
